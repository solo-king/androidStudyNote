参考资料:
    //7.1.2 ntp系统时间更新
    https://blog.csdn.net/yin1031468524/article/details/65447849

NITZ(Network Identity and Time Zone):
NTP(Network Time Protocol):
SNTP():
Android 时间同步有两种方式
    1.通过运营商获取时间，其中运营商提供时间和时区，用的是 NITZ 协议
    2.通过网络获取时间，用的是 SNTP 协议（NTP 协议的简单版本）
源码分析:
涉及源码:
frameworks/opt/telephony/src/java/com/android/internal/telephony/ServiceStateTracker.java
packages/apps/Settings/src/com/android/settings/DateTimeSettings.java
frameworks/base/services/core/java/com/android/server/NetworkTimeUpdateService.java 
res/values-zh-rCN/strings.xml:294:    <string name="date_time_auto" msgid="7076906458515908345">"自动确定日期和时间"</string>
        res/xml/date_time_prefs.xml:23:        android:title="@string/date_time_auto"
            <com.android.settingslib.RestrictedSwitchPreference android:key="auto_time"
                android:title="@string/date_time_auto"
                android:summaryOn="@string/date_time_auto_summaryOn"
                android:summaryOff="@string/date_time_auto_summaryOff"
                settings:useAdditionalSummary="true"
                settings:restrictedSwitchSummary="@string/enabled_by_admin"
            />
        src/com/android/settings/DateTimeSettings.java:94:        addPreferencesFromResource(R.xml.date_time_prefs);
        private static final String KEY_AUTO_TIME = "auto_time";
        mAutoTimePref = (RestrictedSwitchPreference) findPreference(KEY_AUTO_TIME);
Settings中设置"自动更新日期和时间",导致系统时间更新
    switch的初始化(都在DateTimeSettings.java中):
        onCreate
            initUI
                /* 初始化switch控件 */
                mAutoTimePref = (RestrictedSwitchPreference) findPreference(KEY_AUTO_TIME);
                /*
                    注册 OnPreferenceChangeListener 监听器,点按switch后,会触发 OnPreferenceChangeListener
                */
                mAutoTimePref.setOnPreferenceChangeListener(this);
    switch事件处理(都在DateTimeSettings.java中):
        onPreferenceChange
            /*
                根据当前switch值,对Settings.Global.AUTO_TIME数据库的至进行设置
                    1.如果switch被打开,则设置其为1,否则为0
            */
            Settings.Global.putInt(getContentResolver(), Settings.Global.AUTO_TIME,autoEnabled ? 1 : 0);
            /*自动更新时区被打开后,禁用手动设置时间及日期 list */
            mTimePref.setEnabled(!autoEnabled);
            mDatePref.setEnabled(!autoEnabled);
    注册Settings.Global.AUTO_TIME 值监听器(ServiceStateTracker):
        ServiceStateTracker 的初始化(此处以GsmCdmaPhone.java为例子):
            GsmCdmaPhone
                ServiceStateTracker mSST = mTelephonyComponentFactory.makeServiceStateTracker(this/*GsmCdmaPhone本身*/, this.mCi);
        /*在ServiceStateTracker对象被创建的时候注册监听器mAutoTimeObserver*/
        ServiceStateTracker
            mCr.registerContentObserver(
                    Settings.Global.getUriFor(Settings.Global.AUTO_TIME), true,
                    mAutoTimeObserver);
        /*
            监听器如下
            当Settings.Global.AUTO_TIME值发生变化后,mAutoTimeObserver将被getContentResolver回调,从而执行
                revertToNitzTime();
        */
       private ContentObserver mAutoTimeObserver = new ContentObserver(new Handler()) {
            @Override
            public void onChange(boolean selfChange) {
                Rlog.i(LOG_TAG, "Auto time state changed");
                revertToNitzTime();
            }
        };
    时钟更新具体实现:
        A1.revertToNitzTime
            /*
                接口解读:
                    SystemClock.elapsedRealtime()
                        Returns milliseconds since boot, including time spent in sleep
                mSavedTime:
                    现在系统从开机到现在已记录的时间,单位为毫秒
                mSavedAtTime:
                    上一次记录的系统从开机到现在已记录的时间,单位为毫秒
                    所以这里(SystemClock.elapsedRealtime() - mSavedAtTime)含义为增量值
                总结:setAndBroadcastNetworkSetTime(xxx),最终发送的值为发送此广播时的真正启动时间,含休眠时间
            */
            B1.setAndBroadcastNetworkSetTime(mSavedTime
                    + (SystemClock.elapsedRealtime() - mSavedAtTime));
                //1.回设置硬件时间
                C1.SystemClock.setCurrentTimeMillis(time);
                /*
                    1.填充intent
                        Intent intent = new Intent(TelephonyIntents.ACTION_NETWORK_SET_TIME);
                        intent.addFlags(Intent.FLAG_RECEIVER_REPLACE_PENDING);
                        intent.putExtra("time", time);
                    2.此处的mPhone已GsmCdmaPhone.java为例子,此处的Context为PhoneGlobals对象,继承于ContextWrapper
                        mPhone.getContext().sendStickyBroadcastAsUser(intent, UserHandle.ALL);
                */
                C2.将time值通过发送到符合intent的广播接收器中
    注册TelephonyIntents.ACTION_NETWORK_SET_TIME 的广播接收器(NetworkTimeUpdateService.java):
        systemRunning
            registerForTelephonyIntents
                intentFilter.addAction(TelephonyIntents.ACTION_NETWORK_SET_TIME);
                intentFilter.addAction(TelephonyIntents.ACTION_NETWORK_SET_TIMEZONE);
                mContext.registerReceiver(mNitzReceiver, intentFilter);
        对于ACTION_NETWORK_SET_TIME的处理方法:
            /** 
             * 1.对本地mNitzTimeSetTime值进行设置,最终在 onPollNetworkTimeUnderWakeLock 中处理
             * 2.从 onPollNetworkTimeUnderWakeLock的处理中也能看出 NITZ的使用优先级大于 NTP 的方式
             */
            private BroadcastReceiver mNitzReceiver = new BroadcastReceiver() {
                @Override
                public void onReceive(Context context, Intent intent) {
                    String action = intent.getAction();
                    if (DBG) Log.d(TAG, "Received " + action);
                    if (TelephonyIntents.ACTION_NETWORK_SET_TIME.equals(action)) {
                        mNitzTimeSetTime = SystemClock.elapsedRealtime();
                    } else if (TelephonyIntents.ACTION_NETWORK_SET_TIMEZONE.equals(action)) {
                        mNitzZoneSetTime = SystemClock.elapsedRealtime();
                    }
                }
            };
    NetworkTimeUpdateService 服务的注册及启动:
        通过config.disable_network及config.disable_networktime属性来控制是否启动该服务:
            boolean disableNetwork = SystemProperties.getBoolean("config.disable_network", false);
            disableNetworkTime = SystemProperties.getBoolean("config.disable_networktime", false);
        涉及源码:
            frameworks/base/services/java/com/android/server/SystemServer.java
            frameworks/base/core/res/
        源码分析:
        startOtherServices
            /*
                1.设置远程ntp服务器
                2.获取闹钟服务
                    mAlarmManager = (AlarmManager) mContext.getSystemService(Context.ALARM_SERVICE);
                3.设置一个轮服务
                    Intent pollIntent = new Intent(ACTION_POLL, null);
                    mPendingPollIntent = PendingIntent.getBroadcast(mContext, POLL_REQUEST, pollIntent, 0);
                4.从framework-res.apk中获取一些初始值,用于控制服务策略
                    mPollingIntervalMs:com.android.internal.R.integer.config_ntpPollingInterval
                        Normal polling frequency in milliseconds
                    mPollingIntervalShorterMs:com.android.internal.R.integer.config_ntpPollingIntervalShorter
                        Try-again polling interval in milliseconds, in case the network request failed
                    mTryAgainTimesMax:com.android.internal.R.integer.config_ntpRetry
                        Number of times to try again with the shorter interval, before backing off until the normal polling interval. A value < 0 indicates infinite.
                    mTimeErrorThresholdMs:com.android.internal.R.integer.config_ntpThreshold
                        If the time difference is greater than this threshold in milliseconds,then update the time.
                5.获取一个唤醒锁用于禁用系统休眠
                    mWakeLock = ((PowerManager) context.getSystemService(Context.POWER_SERVICE)).newWakeLock(PowerManager.PARTIAL_WAKE_LOCK, TAG);
            */
            networkTimeUpdater = new NetworkTimeUpdateService(context);//NetworkTimeUpdateService networkTimeUpdaterF = networkTimeUpdater;
            ServiceManager.addService("network_time_update_service", networkTimeUpdater);
            /*
                同一让mActivityManagerService去启动这些耗时服务,其中就包括 networkTimeUpdaterF = networkTimeUpdater 
            */

            mActivityManagerService.systemReady(new Runnable()
                /* 
                    1.注册必要的广播接收器,以实现如下几种事件时调用 onPollNetworkTime(msg.what)对系统时间进行更新
                        1.1 telephone模块发出 TelephonyIntents.ACTION_NETWORK_SET_TIME/TelephonyIntents.ACTION_NETWORK_SET_TIMEZONE广播,此处将是NITZ方式
                        1.2 ACTION_POLL 广播,广播服务将定时的发出POLL广播
                        1.3 网络发生变化后
                            ConnectivityManager.CONNECTIVITY_ACTION
                    2.创建一个 MyHandler对事件进行处理
                        mHandler = new MyHandler(thread.getLooper());
                    3.触发一次更新时间及时区
                        mHandler.obtainMessage(EVENT_POLL_NETWORK_TIME).sendToTarget();
                    4.注册一个ContentObserver观察者,在Settings.Global.AUTO_TIME值发生变化的时候调用onChange处理
                        mSettingsObserver = new SettingsObserver(mHandler, EVENT_AUTO_TIME_CHANGED);
                */
                networkTimeUpdaterF.systemRunning();
    NetworkTimeUpdateService更新系统时间/时区:
        /*
            event值可取:
                EVENT_AUTO_TIME_CHANGED:自动设置时间按钮值发生变化
                EVENT_POLL_NETWORK_TIME:定时检查,通过poll机制
                EVENT_NETWORK_CHANGED:网络发生变化时
        */
        onPollNetworkTime(event)
            /*
                1.上一次基站获取的时间与现在真正的系统时间差值是否在poll间隔内,位于poll间隔内(该值从framework-res.apk中获取)说明已经被nitz方式更新过了,否则说明从基站获取的时钟存在问题
                    mNitzTimeSetTime != NOT_SET && refTime - mNitzTimeSetTime < mPollingIntervalMs
                2.不满足1的情况后走这条,如下几种情况进行ntp方式更新系统时间
                    2.1 mLastNtpFetchTime未被设置
                    2.2 当前的时间大于上一次记录的ntp时间+poll间隔
                    2.3 用户点击了Settings中的更新按钮的值
                        event == EVENT_AUTO_TIME_CHANGED
                3.如果缓存时间间隔大于 poll间隔,说明当前时间存在用户期望的异常了,则需要与ntp进行比对时间了
                    mTime.forceRefresh();
                4.更新系统时间
                    SystemClock.setCurrentTimeMillis(ntp);
            */
            onPollNetworkTimeUnderWakeLock(event);
    Android中的闹钟机制:
        涉及源码:
            frameworks/base/core/java/android/app/SystemServiceRegistry.java
        服务的注册:
            registerService(Context.ALARM_SERVICE, AlarmManager.class,
                new CachedServiceFetcher<AlarmManager>() {
            @Override
            public AlarmManager createService(ContextImpl ctx) {
                IBinder b = ServiceManager.getService(Context.ALARM_SERVICE);
                IAlarmManager service = IAlarmManager.Stub.asInterface(b);
                return new AlarmManager(service, ctx);
            }});
        服务的获取:
            AlarmManager mAlarmManager = (AlarmManager) mContext.getSystemService(Context.ALARM_SERVICE);   
        
    NetworkTimeUpdateService中的poll机制:
        /**/
        Intent pollIntent = new Intent(ACTION_POLL, null);
        mPendingPollIntent = PendingIntent.getBroadcast(mContext, POLL_REQUEST, pollIntent, 0);

        //将一个等待PendingIntent从ALARM_SERVICE服务中的等待队列中移去
        mAlarmManager.cancel(mPendingPollIntent);
        //将新PendingIntent添加到从ALARM_SERVICE服务中的等待队列中
        mAlarmManager.set(AlarmManager.ELAPSED_REALTIME, next, mPendingPollIntent);
                




        
        
        
                


            










