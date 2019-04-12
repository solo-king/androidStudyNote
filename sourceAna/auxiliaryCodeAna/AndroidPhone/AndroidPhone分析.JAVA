基础知识:
    HandlerThread()的使用

Phone应用的初始化:
    //对于Android2.3版本
    makeDefaultPhones所做事项(PhoneFactory.java):
        1.创建RIL对象
        2.获取phone类型
        3.根据phone类型创建GSMPhone或者CDMAPhone代理
    RIL构造函数所处理的事情:
        1.创建一个HandlerThread("RILSender")线程
        2.创建一个Thread(mRILRecever,"RILRecever")线程
            2.1 mRILRecever线程所处理的事情
                2.1.1 与rild建立socket链接
                2.1.2 不断的通过socket从rild中获取数据(Parcel p),并调用processRespones(p)进行处理
    RIL对打电话的处理:
        1.创建一个RILRequest rr,并填入号码
        2.调用send(rr)将请求发送出去
            2.1 
apn选择问题:
    基础知识:
            MCC：Mobile Country Code，移动国家码
            MNC:Mobile Network Code，移动网络码
            MSIN:Mobile Subscriber Identification Number共有10位
            mcc及mnc查询官网
                http://www.mcc-mnc.com/
    03-28 11:33:42.147 D/DCT     (  618): [0]getPreferredApn: X not found
    问题原因:
    涉及指令:
        getprop gsm.sim.operator.numeric
        logcat -s ApnSettings&
        echo -e "at+copn=?\r\n" > /dev/ttyUSB2
        numeric="46011" AND NOT (type='ia' AND (apn="" OR apn IS NULL)) AND user_visible!=0
        content query --uri content://telephony/carriers --where "numeric="46011" AND NOT (type='ia' AND (apn="" OR apn IS NULL)) AND user_visible!=0"
        content query --uri content://telephony/carriers --where "numeric="46011" AND NOT ( (apn='' OR apn IS NULL)) AND user_visible!=0"

        content query --uri content://telephony/carriers --where "numeric="46011" AND NOT (false) AND user_visible!=0"
        content query --uri content://telephony/carriers --where "numeric='46011' AND name='myctnet'"
        content query --uri content://telephony/carriers --where "name='myctnet'"
        content query --uri content://telephony/carriers --where "mcc="460""
        content query --uri content://telephony/carriers/preferapn_no_update
        logcat -s ApnEditor 
    涉及源码:
        packages/services/Telephony/src/com/android/phone/MobileNetworkSettings.java
        frameworks/opt/telephony/src/java/android/provider/Telephony.java
        packages/apps/Settings/src/com/android/settings/ApnSettings.java

    res/values-zh-rCN/strings.xml:2452:    <string name="access_point_names" msgid="1381602020438634481">"接入点名称(APN)"</string>
    ps = findPreference(BUTTON_APN_EXPAND_KEY);//

    默认APN选项:
    frameworks/opt/telephony/src/java/com/android/internal/telephony/dataconnection/DcTracker.java
        getPreferredApn

1.设置中的APN页面选中圈圈使用的字段
    涉及源码:
        packages/apps/Settings/src/com/android/settings/ApnSettings.java
        fillList
            /*
                1.确认该apn是否可选
                2.mSelectedKey = getSelectedApnKey();
                    从 content://telephony/carriers/preferapn 这个数据库中获取id号为0的作为被选中的key
                        Cursor cursor = getContentResolver().query(PREFERAPN_URI, new String[] {"_id"},
                        null, null, Telephony.Carriers.DEFAULT_SORT_ORDER);
                        if (cursor.getCount() > 0) {
                            cursor.moveToFirst();
                            key = cursor.getString(ID_INDEX);
                        }
            */
            if (selectable) {
                    if ((mSelectedKey != null) && mSelectedKey.equals(key)) {
                        pref.setChecked();
                    }
                    addApnToList(pref, mnoApnList, mvnoApnList, r, mvnoType, mvnoMatchData);
                } 
2.什么情况下会调用 fillList 去填充list
    2.1 接收到该TelephonyIntents.ACTION_ANY_DATA_CONNECTION_STATE_CHANGED广播时
        /**
         * 1.从intent中的字段 PhoneConstants.STATE_KEY 获取当前移动网络链接状态，状态值使用枚举表示
         *  1.1 CONNECTED, CONNECTING, DISCONNECTED, SUSPENDED;
         * private static PhoneConstants.DataState getMobileDataState(Intent intent) {
                String str = intent.getStringExtra(PhoneConstants.STATE_KEY);
                if (str != null) {
                    return Enum.valueOf(PhoneConstants.DataState.class, str);
                } else {
                    return PhoneConstants.DataState.DISCONNECTED;
                }
            }
         */
        if (intent.getAction().equals(//packages/apps/Settings/src/com/android/settings/ApnSettings.java
            TelephonyIntents.ACTION_ANY_DATA_CONNECTION_STATE_CHANGED)) {
            PhoneConstants.DataState state = getMobileDataState(intent);
            switch (state) {
            case CONNECTED:
                if (!mRestoreDefaultApnMode) {//mRestoreDefaultApnMode代表当前重置apn状态，true代表重置中，fals则是重置完成
                    fillList();
                } else {
                    showDialog(DIALOG_RESTORE_DEFAULTAPN);
                }
                break;
            }
        }
    2.2 在该页面进入 onResume阶段是调用
        public void onResume() {
            ...
            if (!mRestoreDefaultApnMode) {
                fillList();
            }
        }
    2.3 在用户设置 “重置默认值”时进行刷新页面
        restoreDefaultApn()
3.圈圈使用的字段是如何被系统服务设置的
        见问题1
4. TelephonyIntents.ACTION_ANY_DATA_CONNECTION_STATE_CHANGED 广播什么情况下发送
    //java代码中所有使用到的地方,删除一些不会发送广播的文件
    ./frameworks/base/services/core/java/com/android/server/TelephonyRegistry.java:1446:        Intent intent = new Intent(TelephonyIntents.ACTION_ANY_DATA_CONNECTION_STATE_CHANGED);
涉及指令:
    logcat -b radio DefaultPhoneNotifier:V TelephonyRegistry:V  PhoneFactory:V  SST:V *:S&
    logcat -b radio SST:V *:S&
    logcat -b radio DefaultPhoneNotifier:V TelephonyRegistry:V  PhoneFactory:V *:S -f /sdcard/phone1.log
    
    

    sPhoneNotifier = new DefaultPhoneNotifier();
    /**
     * 该函数调用的地方在源码中比较频繁
     * 
     */
    notifyDataConnection
        /**
         *调用到的地方
         *  ./frameworks/opt/telephony/src/java/com/android/internal/telephony/DefaultPhoneNotifier.java:184:
         * notifyDataConnection //复写父类
         *   doNotifyDataConnection //DefaultPhoneNotifier.java私有方法
         *      notifyDataConnectionForSubscriber   //DefaultPhoneNotifier.java私有方法
         * ./frameworks/base/services/core/java/com/android/server/TelephonyRegistry.java:1004
         */
        notifyDataConnectionForSubscriber
            broadcastDataConnectionStateChanged//TelephonyRegistry.java私有方法
DefaultPhoneNotifier的初始化:
    ./frameworks/opt/telephony/src/java/com/android/internal/telephony/PhoneFactory.java:130:                sPhoneNotifier = new DefaultPhoneNotifier();
    ./frameworks/opt/telephony/tests/telephonytests/src/com/android/internal/telephony/DefaultPhoneNotifierTest.java:68:        mDefaultPhoneNotifierUT = new DefaultPhoneNotifier();

notifyServiceStateForPhoneId调用:
    ./frameworks/opt/telephony/src/java/com/android/internal/telephony/DefaultPhoneNotifier.java:88:                mRegistry.notifyServiceStateForPhoneId(phoneId, subId, ss);
    /**
     * 1.只在Phone中调用 notifyServiceState
     * ./frameworks/opt/telephony/src/java/com/android/internal/telephony/Phone.java:1418:        mNotifier.notifyServiceState(this);
     * 2.只在ServiceStateTracker中调用 notifyServiceStateChanged
     * ./frameworks/opt/telephony/src/java/com/android/internal/telephony/ServiceStateTracker.java:2786:            mPhone.notifyServiceStateChanged(mSS);
     */
    ./frameworks/opt/telephony/src/java/com/android/internal/telephony/ServiceStateTracker.java:2786:            mPhone.notifyServiceStateChanged(mSS);
    notifyServiceStateChanged   //GsmCdmaPhone中直接父类的notifyServiceStateChangedP方法
        notifyServiceStateChangedP//protected属性，只能在本包中或者继承调用
            notifyServiceState
                mRegistry.notifyServiceStateForPhoneId
                
ActivityManagerNative.getDefault().updateConfiguration(config);

手动拨号:
    crw-rw---- 1 root plugdev 188, 0 3月  29 16:15 /dev/ttyUSB0
    crw-rw---- 1 root plugdev 188, 1 3月  29 16:17 /dev/ttyUSB1
    crw-rw---- 1 root plugdev 188, 0 3月  29 16:15 /dev/ttyUSB0
    crw-rw---- 1 root plugdev 188, 1 3月  29 16:17 /dev/ttyUSB1
    crw-rw---- 1 root plugdev 188, 2 3月  29 16:23 /dev/ttyUSB2
    crw-rw---- 1 root plugdev 188, 3 3月  29 16:23 /dev/ttyUSB3
    crw-rw---- 1 root plugdev 188, 4 3月  29 16:23 /dev/ttyUSB4
    crw-rw---- 1 root plugdev 188, 5 3月  29 16:23 /dev/ttyUSB5
    busybox wget http://119.188.244.32:80/soft/antutu-benchmark-v7-2-7.apk
    echo -e "at\$QCRMCALL=1,1\r\n"> /dev/ttyUSB1