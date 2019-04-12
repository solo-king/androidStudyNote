logcat: 
        
    -v <format>, --format=<format>
        Sets the log print format, where <format> is:
            brief color epoch long monotonic printable process raw
            tag thread threadtime time uid usec UTC year zone
涉及源码:
    hardware/ril/include/telephony/ril.h
    

重要结构体:
    typedef struct {
        int version;        /* set to RIL_VERSION */
        RIL_RequestFunc onRequest;
        RIL_RadioStateRequest onStateRequest;
        RIL_Supports supports;
        RIL_Cancel onCancel;
        RIL_GetVersion getVersion;
    } RIL_RadioFunctions;

    struct RIL_Env {
        void (*OnRequestComplete)(RIL_Token t, RIL_Errno e,
                            void *response, size_t responselen);
    #if defined(ANDROID_MULTI_SIM)
        void (*OnUnsolicitedResponse)(int unsolResponse, const void *data, size_t datalen, RIL_SOCKET_ID socket_id);
    #else
        void (*OnUnsolicitedResponse)(int unsolResponse, const void *data, size_t datalen);
    #endif
        void (*RequestTimedCallback) (RIL_TimedCallback callback,
                                    void *param, const struct timeval *relativeTime);
        void (*OnRequestAck) (RIL_Token t);
    };
    initializeCallback
        rild库中,用于初始化硬件模块
    mainLoop
        rild库中
一.框架
    1. java 与modem通讯的接口
        java-->|还需补充|rild.c-->pipe-->libreference-rild.so-->modem
    2. rild与上层接口通过哪种方式进行通讯
        socket
        s_rilenv = env;//指向rild.c中的struct RIL_Env结构体
    3. Phone如何创建，其包含那些类
    4. 
    Phone
    Ril.java
问题:
    1.几个log所属
        1.1 I use-Rlog/RLOG-RIL: entering mainLoop()
            hardware/ril/reference-ril-queue/reference-ril.c
                AT_DUMP("== ", "entering mainLoop()", -1 );
        1.2  D RILD    : **RIL Daemon Started**
            hardware/ril/rild/rild.c
                main
                    RLOGD("**RIL Daemon Started**");
        1.3 D RILC    : Quectel RIL Version: Quectel_Android_RIL_Driver_V1.41.36
            hardware/ril/reference-ril-queue/reference-ril.c
                RIL_Init
                    LOGD("Quectel RIL Version: " REFERENCE_RIL_VERSION);
        1.4 E RIL_READ_CONF: '/system/etc/ql-ril.conf' not exist.
        1.5 D RILU    : find quectel module /sys/bus/usb/devices/1-1 idVendor=2c7c idProduct=0125
            hardware/ril/reference-ril-queue/ql-usb.c
            LOGD("find quectel module %s idVendor=%s idProduct=%s", usb_device_info.usbdevice_pah, idVendor, idProduct);
        1.6  E ATC     : at_open s_tild_reader = -1418200800
            hardware/ril/reference-ril-queue/atchannel.c
                LOGE("at_open s_tild_reader = %ld", s_tid_reader);
        1.7  D TelephonyManager: No /proc/cmdline exception=java.io.FileNotFoundException: /proc/cmdline (Permission denied)
        1.8 D TDC     : updateOrInsert: inserting: Modem { uuid=modem, state=0, rilModel=0, rat={}, maxActiveVoiceCall=1, maxActiveDataCall=1, maxStandby=1 }
        1.9 D CdmaSSM : subscriptionSource from settings: 0
        2.0 D TelephonyRegistry: listen oscl: hasNotifySubscriptionInfoChangedOccurred==false no callback
        2.1 I PhoneFactory: Network Mode set to 9
        2.2  D RILJ    : [3650]< RIL_REQUEST_CDMA_SET_SUBSCRIPTION_SOURCE  [SUB0]
            frameworks/opt/telephony/src/java/com/android/internal/telephony/RIL.java
                class RIL
        2.3  D SubscriptionController: [SubscriptionController] init by Context
        2.4  D UiccController: Creating UiccController
        2.5 622   622 D ProxyController: Constructor - Enter
        2.6 622   622 D SIMRecords: [SIMRecords] getOperatorNumeric: IMSI == null
        2.7 622   622 D SST     : NITZ: 19/01/15,00:58:08+32,0,10448 start=10574 delay=126
        2.8 622   622 D GsmSMSDispatcher: GsmSMSDispatcher: subId = -2 slotId = 0
    2. 代理模式的优势及劣势
    3. 对于移远的库在处理RIL_REQUEST_RADIO_POWER模式时，是如何再调用ppp进行拨号的
            reference-ril.c:5832:        case RIL_REQUEST_RADIO_POWER:
8.设备插入联通卡，首选网络类型设置为3G；然后设备断电，更换电信卡，驻网模式时3G，但是首选网络类型中，选中的是LTE模式。此问题出现了一次
    问题原因:
        每次更新preferred_network_mode1时,都先使用模式的替换了
    //显示ui组件的初始化
    PreferenceScreen prefSet = getPreferenceScreen();
    //BUTTON_ENABLED_NETWORKS_KEY = "enabled_networks_key"
    mButtonEnabledNetworks = (ListPreference) prefSet.findPreference(BUTTON_ENABLED_NETWORKS_KEY);
    涉及源码:
        packages/services/Telephony/src/com/android/phone/MobileNetworkSettings.java
        res/values-zh-rCN/strings.xml:2455:    <string name="preferred_network_type_title" msgid="3431041717309776341">"首选网络类型"</string>
        packages/services/Telephony/res/xml/network_setting.xml:40:        android:key="enabled_networks_key"
        packages/services/Telephony/src/com/android/phone/MobileNetworkSettings.java:436:        addPreferencesFromResource(R.xml.network_setting);
    源码分析:
        1. 网络首选项点击后从MobileNetworkSettings@onPreferenceChange@preference == mButtonEnabledNetworks 处执行
            mPhone.setPreferredNetworkType(modemNetworkMode, mHandler.obtainMessage(MyHandler.MESSAGE_SET_PREFERRED_NETWORK_TYPE));
        NetworkSettings: settingsNetworkMode: 1 
        2. 初始化时更新控件
            updateBody
    涉及指令:
        logcat -s NetworkSettings&
        logcat -s NetworkSettings
        vi /data/system/users/0/settings_global.xml
5.设备通过断电关机重启，出现两次VPDN参数丢失情况。
    基础知识:
        MCC：Mobile Country Code，移动国家码
        MNC:Mobile Network Code，移动网络码
        MSIN:Mobile Subscriber Identification Number共有10位
        mcc及mnc查询官网
            http://www.mcc-mnc.com/
    问题原因:
        获取mccmnc问题,有时获取到的为 46003,有时为46011
        1.mccmnc的获取
            //final TelephonyManager tm = (TelephonyManager) getSystemService(Context.TELEPHONY_SERVICE);
            final String mccmnc = mSubscriptionInfo == null ? "": tm.getSimOperator(mSubscriptionInfo.getSubscriptionId());
                getSimOperatorNumeric();
                    getSimOperatorNumeric(subId);
                        int phoneId = SubscriptionManager.getPhoneId(subId);
                        getSimOperatorNumericForPhone(phoneId);
                            //PROPERTY_ICC_OPERATOR_NUMERIC == gsm.sim.operator.numeric
                            getTelephonyProperty(phoneId,TelephonyProperties.PROPERTY_ICC_OPERATOR_NUMERIC, "");
        2.mccmnc的设置
            setSimOperatorNumericForPhone()//TelephontManager.java
                setTelephonyProperty(phoneId,TelephonyProperties.PROPERTY_ICC_OPERATOR_NUMERIC, numeric);

            setSimOperatorNumeric(String numeric)
                int phoneId = getDefaultPhone();
                setSimOperatorNumericForPhone(phoneId, numeric);
            //调用setSimOperatorNumericXXX对numeric设置的地方
            frameworks/opt/telephony/src/java/com/android/internal/telephony/GsmCdmaPhone.java +308
            frameworks/opt/telephony/src/java/com/android/internal/telephony/uicc/IccCardProxy.java  +248  +356
            frameworks/opt/telephony/src/java/com/android/internal/telephony/uicc/RuimRecords.java +771 
            frameworks/opt/telephony/src/java/com/android/internal/telephony/uicc/SIMRecords.java  +288 +1568
    涉及指令:
        getprop gsm.sim.operator.numeric
        logcat -s ApnSettings&
        Uri CONTENT_URI = Uri.parse("content://telephony/carriers");//Telephony.Carriers.CONTENT_URI
        StringBuilder where = new StringBuilder("numeric=\"" + mccmnc +
                "\" AND NOT (type='ia' AND (apn=\"\" OR apn IS NULL)) AND user_visible!=0");
        numeric="46011" AND NOT (type='ia' AND (apn="" OR apn IS NULL)) AND user_visible!=0
        content query --uri content://telephony/carriers --where "numeric="46011" AND NOT (type='ia' AND (apn="" OR apn IS NULL)) AND user_visible!=0"
        content query --uri content://telephony/carriers --where "numeric="46011" AND NOT ( (apn='' OR apn IS NULL)) AND user_visible!=0"

        content query --uri content://telephony/carriers --where "numeric="46011" AND NOT (false) AND user_visible!=0"
        content query --uri content://telephony/carriers --where "numeric="46011""
        content query --uri content://telephony/carriers --where "mcc="460""
        content query --uri content://telephony/carriers/preferapn
    涉及源码:
        packages/services/Telephony/src/com/android/phone/MobileNetworkSettings.java
        frameworks/opt/telephony/src/java/android/provider/Telephony.java
        packages/apps/Settings/src/com/android/settings/ApnSettings.java

    res/values-zh-rCN/strings.xml:2452:    <string name="access_point_names" msgid="1381602020438634481">"接入点名称(APN)"</string>
    ps = findPreference(BUTTON_APN_EXPAND_KEY);//
    //与下面log对应
    Row: 0 _id=1104, name=China Telecom,       numeric=46011, mcc=460, mnc=11, apn=CTLTE,        user=ctnet@mycdma.cn,       server=, password=vnet.mobi,  proxy=,          port=,   mmsproxy=,           mmsport=,   mmsc=,                      authtype=-1, type=default, current=1, protocol=IP, roaming_protocol=IP, carrier_enabled=1, bearer=0, bearer_bitmask=0, mvno_type=, mvno_match_data=, sub_id=5, profile_id=0, modem_cognitive=0, max_conns=0, wait_time=0, max_conns_time=0, mtu=0, edited=0, user_visible=1
    Row: 1 _id=1105, name=China Telecom wap,   numeric=46011, mcc=460, mnc=11, apn=CTWAP,        user=ctwap@mycdma.cn,       server=, password=vnet.mobi,  proxy=,          port=,   mmsproxy=10.0.0.200, mmsport=80, mmsc=http://mmsc.vnet.mobi, authtype=-1, type=default, current=1, protocol=IP, roaming_protocol=IP, carrier_enabled=1, bearer=0, bearer_bitmask=0, mvno_type=, mvno_match_data=, sub_id=5, profile_id=0, modem_cognitive=0, max_conns=0, wait_time=0, max_conns_time=0, mtu=0, edited=0, user_visible=1
    Row: 2 _id=1106, name=China Telecom Mms,   numeric=46011, mcc=460, mnc=11, apn=CTWAP,        user=ctwap@mycdma.cn,       server=, password=vnet.mobi,  proxy=10.0.0.200,port=80, mmsproxy=10.0.0.200, mmsport=80, mmsc=http://mmsc.vnet.mobi, authtype=-1, type=mms,     current=1, protocol=IP, roaming_protocol=IP, carrier_enabled=1, bearer=0, bearer_bitmask=0, mvno_type=, mvno_match_data=, sub_id=5, profile_id=0, modem_cognitive=0, max_conns=0, wait_time=0, max_conns_time=0, mtu=0, edited=0, user_visible=1
    Row: 3 _id=1408, name=vpdn,                numeric=46011, mcc=460, mnc=11, apn=sdyr.vpdn.sd, user=usrtest1@sdyr.vpdn.sd, server=, password=www.usr.cn, proxy=,          port=,   mmsproxy=,           mmsport=,   mmsc=,                      authtype=-1, type=,        current=1, protocol=IP, roaming_protocol=IP, carrier_enabled=1, bearer=0, bearer_bitmask=0, mvno_type=, mvno_match_data=, sub_id=5, profile_id=0, modem_cognitive=0, max_conns=0, wait_time=0, max_conns_time=0, mtu=0, edited=1, user_visible=1


