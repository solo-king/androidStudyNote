基础知识:
    状态机的使用:
        1.添加一个状态至状态机中
            addState(mDefaultState);
        2.启动状态机
            1.setInitialState:
                设置一个状态机的初始状态
            2.mWifiController.start();
                启动一个状态机
        3.发送一个消息至状态机中
            3.1 obtainMessage
            3.2 sendMessage
        3.切换状态机状态
            transitionTo
        4.状态处理完成返回值
            HANDLED
            NOT_HANDLED
     一个消息在一个状态机中的处理过程:
        1.先调用当前状态的processMessage
        2.如果子状态返回NOT_HANDLED or false那么将调用他的父状态的processMessage进行处理
        3.如果当前状态及其所有父状态都无法处理该Message，那么将调用状态机中的unhandledMessage进行最后一次处理，再失败则丢弃该消息\

Tethering概念:
    Tethering means sharing the Internet connection of an Internet-capable mobile phone with other devices. 
    This sharing can be offered over a wireless LAN (Wi-Fi), or over Bluetooth, or by physical connection using a cable (Tethering). 
参考资料:
    WIFI系列://讲的一般般细节框架都有涉及但不够详细
        https://blog.csdn.net/u011913612/article/category/6455481
        AndroidM中wpa_client通过socket与wpa_supplican链接交互
    AndroidN上wifi模块的分析：
        https://blog.csdn.net/mr_55/article/details/79030932
        
        WifiManager：WifiService的客户端，是App层与Framework层之间的桥梁。
        （代码位置：frameworks/base/wifi/java/android/net/wifi/WifiManager.java）
        WifiService：是Framework层负责wifi功能的核心服务，是系统进程。
        （代码位置：frameworks/opt/net/wifi/service/java/com/android/server/wifi/WifiServiceImpl.java）
        WifiStateMachine：wifi状态机，WifiService的核心。
        （代码位置：frameworks/opt/net/wifi/service/java/com/android/server/wifi/WifiStateMachine.java）
        WifiMonitor：一个线程，用来接收来自WPAS的消息。
        （代码位置：frameworks/opt/net/wifi/service/java/com/android/server/wifi/WifiMonitor.java）
        WifiNative：用于和WPAS通信，内部定义许多native方法，对应的JNI模块是android_net_wifi_Wifi。
        （代码位置：frameworks/opt/net/wifi/service/java/com/android/server/wifi/WifiNative.java
涉及指令:
    logcat TetheringSettings:V TetherProvisioningAct:V *:S&
log分析:
    public static final int TETHERING_WIFI      = 0;
    12-21 14:03:26.838 22064 22064 D TetheringSettings: onPreferenceChange:[enable]true
    12-21 14:03:26.838 22064 22064 D TetheringSettings: startTethering:[choice]0
涉及源码:
    frameworks/opt/net/wifi/service/java/com/android/server/wifi/WifiNative.java
    frameworks/opt/net/wifi/service/jni/com_android_server_wifi_WifiNative.cpp
1.ConnectivityService的初始化
/**
networkStats = NetworkStatsService.create(context, networkManagement);
ServiceManager.addService(Context.NETWORK_STATS_SERVICE, networkStats);
*/
connectivity = new ConnectivityService(
                            context, networkManagement, networkStats, networkPolicy);
                    ServiceManager.addService(Context.CONNECTIVITY_SERVICE, connectivity);
                    networkStats.bindConnectivityManager(connectivity);
                    networkPolicy.bindConnectivityManager(connectivity);
2.mTethering的初始化分析,于ConnectivityService.java中
    ConnectivityService(...)
        ...
        /*
        mNetd:初始化如下于SystemServer.java中
                    networkManagement = NetworkManagementService.create(context);
                    ServiceManager.addService(Context.NETWORKMANAGEMENT_SERVICE, networkManagement);
        statsService: 初始化如下于SystemServer.java中
                    networkStats = NetworkStatsService.create(context, networkManagement);
                    ServiceManager.addService(Context.NETWORK_STATS_SERVICE, networkStats);
        mPolicyManager:初始化如下于SystemServer.java中
                    networkPolicy = new NetworkPolicyManagerService(context, mActivityManagerService, networkStats, networkManagement);
                    ServiceManager.addService(Context.NETWORK_POLICY_SERVICE, networkPolicy);
        */
        A1.mTethering = new Tethering(mContext, mNetd, statsService, mPolicyManager);
             //保存networkManagement networkStats networkPolicy必要对象
             //暂且理解为State Manager/Machine-->状态机
             mTetherMasterSM = new TetherMasterSM("TetherMaster", mLooper);
             //供网设备监视器
             mUpstreamNetworkMonitor = new UpstreamNetworkMonitor();
             //注册一个广播接收器，并设置一系列接收事件
             mStateReceiver = new StateReceiver();
             IntentFilter filter = new IntentFilter();
             filter.addAction(UsbManager.ACTION_USB_STATE);
             filter.addAction(ConnectivityManager.CONNECTIVITY_ACTION);
             filter.addAction(WifiManager.WIFI_AP_STATE_CHANGED_ACTION);
             filter.addAction(Intent.ACTION_CONFIGURATION_CHANGED);
             mContext.registerReceiver(mStateReceiver, filter);
             updateConfiguration();
                /**
                    资源说明:
                        frameworks/base/core/res/res/values/config.xml
                            config_tether_upstream_types: 决定哪些网络可以作为热点的供网设备
                 */
                 mUpstreamIfaceTypes = upstreamIfaceTypes;//能作为热点供网设备的代号集合
                 //check if the upstream type list needs to be modified due to secure-settings
                 checkDunRequired(); 
            //设置默认DNS
            mDefaultDnsServers[0] = DNS_DEFAULT_SERVER1;//8.8.8.8
            mDefaultDnsServers[1] = DNS_DEFAULT_SERVER2;//8.8.4.4
        A2.mNetd.registerObserver(mTethering);

3.根据Settings中的显示字段确定代码
    /packages/apps/Settings/res/values-zh-rCN
    <string name="wifi_tether_checkbox_text" msgid="1847167643625779136">"便携式WLAN热点"</string>
    xml/tether_prefs.xml:26:        android:title="@string/wifi_tether_checkbox_text" />
    //tether_prefs.xml
    <SwitchPreference
            android:key="enable_wifi_ap"
            android:title="@string/wifi_tether_checkbox_text" />
    src/com/android/settings/TetherSettings.java:135:        addPreferencesFromResource(R.xml.tether_prefs);

4.Settings中开启WLAN热点的过程分析,mEnableWifiAp初始化:
    直接查看uml时序图<tethering/startTethering>
WifiController中的几种状态机：
    ApStaDisabledState:
        case CMD_SET_AP:
                    if (msg.arg1 == 1) {
                        if (msg.arg2 == 0) { // previous wifi state has not been saved yet
                            mSettingsStore.setWifiSavedState(WifiSettingsStore.WIFI_DISABLED);
                        }
                        mWifiStateMachine.setHostApRunning((WifiConfiguration) msg.obj,
                                true);
                        transitionTo(mApEnabledState);
                    }
                    break;
    StaEnabledState:
        case CMD_SET_AP:
                    if (msg.arg1 == 1) {
                        // remeber that we were enabled
                        mSettingsStore.setWifiSavedState(WifiSettingsStore.WIFI_ENABLED);
                        deferMessage(obtainMessage(msg.what, msg.arg1, 1, msg.obj));
                        transitionTo(mApStaDisabledState);
                    }
                    break;
    StaDisabledWithScanState:
        case CMD_SET_AP:
                    // Before starting tethering, turn off supplicant for scan mode
                    if (msg.arg1 == 1) {
                        mSettingsStore.setWifiSavedState(WifiSettingsStore.WIFI_DISABLED);
                        deferMessage(obtainMessage(msg.what, msg.arg1, 1, msg.obj));
                        transitionTo(mApStaDisabledState);
                    }
                    break;
    ApEnabledState:
        case CMD_SET_AP:
                    if (msg.arg1 == 0) {
                        mWifiStateMachine.setHostApRunning(null, false);
                        mPendingState = getNextWifiState();
                    }
                    break;
    D NetworkManagement: startAccessPoint:[args1]softap set wlan0 AndroidAP broadcast 1 wpa2-psk 67e5cd06c085 chenqigan
    D NetworkManagement: startAccessPoint:[args2]softap startap chenqigan
NativeDaemonConnector中存在的疑问:
    1.返回值是如何获取的,如下接口中的event = mResponseQueue.remove(sequenceNumber, timeoutMs, logCmd);需如何解读
        public NativeDaemonEvent[] executeForList(long timeoutMs, String cmd, Object... args)
    2.如何执行命令的
                    
