参考资料：
    //对移动网络链接做的说明
    https://source.android.com/devices/tech/config/carrier
    //android rild 结构解释的比较好的
    https://wladimir-tm4pda.github.io/porting/telephony.html

涉及文件:
    hardware/ril/rild/rild.c
    ril/include/telephony/ril.h：
        代表ril所需的接口
RIL 交互方式:
    1.Solicited commands(ril主动像bp发起的请求): 
        Solicited commands originated by RIL lib, such as DIAL and HANGUP.
    2.Unsolicited responses(ril被动的被bp调用): 
        Unsolicited responses that originate from the baseband, such as CALL_STATE_CHANGED and NEW_SMS.

RIL Solicited 类型的请求:
    1.所有的solicited类型求情都以 RIL_REQUEST_ 为前缀，并位于ril/include/telephony/ril.h中
RIL unSolicited 类型的请求:
    1.所有的solicited类型求情都以 RIL_UNSOL_RESPONSE_ 为前缀，并位于ril/include/telephony/ril.h中

##########################################################################移植相关############################################
rild 库必须实现的接口
    1.RIL_RadioFunctions *RIL_Init (RIL_Env* env, int argc, char **argv);
    2.rild通过返回的 RIL_RadioFunctions 结构体来像rild库发送请求
        type structure {
            int RIL_version;
            RIL_RequestFunc onRequest;
            RIL_RadioStateRequest onStateRequest;
            RIL_Supports supports;
            RIL_Cancel onCancel;
            RIL_GetVersion getVersion;
        } 
        RIL_RadioFunctions;
    3.
涉及指令:
    echo -e "AT+CLIP?\r\n" > /dev/ttyUSB1
    echo -e "AT+COPS?\r\n" > /dev/ttyUSB1
    logcat -v brief -b radio RIL:V RILD:V RILJ:V RILC:V AT:V *:S&
    //杀掉rild后台进程
    kill -9 $(ps|grep rild|busybox awk '{print $2}')
LOG对应模块：
    RIL：原生动态库
    RILD:rild.c
    RILJ:
    AT:原生与BP通讯的log
############################################################ril相关#####################################################
总结:
    1.所有通过写到rild socket节点的请求最终都使用ril.cpp中的 processCommandsCallback 进行处理

涉及基础:
    1.定时器的使用:
        struct timeval timeout
    2.select检测多路io
    3.pipe管道的使用
        //初始化一个监控头
        fd_set readFds
        FD_ZERO(&readFds)
        fcntl(s_fdWakeupRead, F_SETFL, O_NONBLOCK);
        //将ev->fd加入到select 监控的fd中
        FD_SET(ev->fd, &readFds);
        n = select(nfds, &rfds, NULL, NULL, ptv);
    4.链表的操作
后续需要整理成笔记的:
    1.rild库中超时任务的实现
    2.rild socket是如何与rild.java进行通讯的
    3.phone应用dail事件的触发
涉及源码:
    hardware/ril/rild/rild.c
/**
 * 获取 ril的库中的 RIL_Init函数
 * 函数定义:
 *  rilInit =(const RIL_RadioFunctions *(*)(const struct RIL_Env *, int, char **))
 * dlHandle:库在内存中的指针
 * RIL_Init:要从库中获取的函数指针
 */
dlHandle = dlopen(rilLibPath, RTLD_NOW);
/**
 * 
 * 1.创建一个 eventLoop 子线程，代码位于libril/ril.cpp
 * 
 *  */
RIL_startEventLoop();
dlsym(dlHandle, "RIL_Init");//rild.c
/**调用动态库中RIL_Init函数
 * const RIL_RadioFunctions *funcs
 * 
 */

funcs = rilInit(&s_rilEnv, argc, rilArgv);
/**
 * 参数:
 *  args=/system/bin/rild -d /dev/ttyUSB2 -c 0
 * 进入rild库
 * 确定参数:
 *  s_device_path = /dev/ttyUSB2
 * 创建线程
 *  mainLoop
 */
ret = pthread_create(&s_tid_mainloop, &attr, mainLoop, NULL);   
//返回一个 RIL_RadioFunctions s_callbacks 供rild.c 使用
return s_callbacks

eventLoop子线程处理
重要成员:
    struct rild_event pending_list
    struct rild_event timer_list
eventLoop
    /**
     * 1.初始化一个select fd头
     *      FD_ZERO(&readFds);
     * 2.
     */
    ril_event_init()
    /**
     * 1.将s_wakeupfd_event挂到 select的监视队列中,只要对应fd发生变化则调用 s_wakeupfd_event所在的rild_event->func进行处理(processWakeupCallback)
     *  ril_event_add(ev);
     * 2.触发 processWakeupCallback
     *  triggerEvLoop();
     */
    rilEventAddWakeup (&s_wakeupfd_event);
mainLoop子线程处理
/**
 * 1.打开串口
 *  fd = open (s_device_path, O_RDWR);
 * 2.将其设置为终端
 *  struct termios  ios;
 *  tcgetattr( fd, &ios );
 *  ios.c_lflag = 0;//disable ECHO, ICANON, etc...
 *  tcsetattr( fd, TCSANOW, &ios );
 *  */
mainLoop
    /**
     * 1.设置solicited事件处理函数
     *      s_unsolHandler = h =onUnsolicited;
     * 2.创建一个read线程
     *      ret = pthread_create(&s_tid_reader, &attr, readerLoop, &attr);
     */
    ret = at_open(fd, onUnsolicited);//hardware/ril/reference-ril/atchannel.c
    /**挂载一个定时调用到rild中去
     * rild将调用 initializeCallback 对 BP进行初始化操作
     * 
     */
    RIL_requestTimedCallback(initializeCallback, NULL, &TIMEVAL_0);

readerLoop子线程
/**
 * 循环子线程
 */
readerLoop
    for:
        //对于短消息处理
        isSMSUnsolicited(line)
        //非短消息分支
        processLine(line);
    /**
     * 1.只有readerLoop所在子线程退出才会执行
     * 2.
     */
    onReaderClosed();
socket rild端口的监听
RIL_register(funcs);//rild.c-->ril.cpp
    
    startListen(RIL_SOCKET_1, &s_ril_param_socket);
        //根据RIL_SOCKET_1设置socket_name = rild, 最终返回 "rild"
        strncpy(socket_name, RIL_getRilSocketName(), 9);
        /**
         * 
         */
        fdListen = android_get_control_socket(socket_name);
        ret = listen(fdListen, 4);
        //这里的 socket_listen_p == s_ril_param_socket
        socket_listen_p->fdListen = fdListen;
        /**
         * 设置一个初始化一个ril_event事件
         *  listen_event->fd = fdListen;
            listen_event->index = -1;
            listen_event->persist = false;
            listen_event->func = listenCallback;
            listen_event->param = socket_listen_p;
            fcntl(listen_event->fd, F_SETFL, O_NONBLOCK);
         */
        ril_event_set (socket_listen_p->listen_event, fdListen, false,
                listenCallback, socket_listen_p);
        /**
         * 1.把 rild socket句柄添加到readFds中，select监视列表中去
         *  ril_event_add()
         *      FD_SET(ev->fd, &readFds);
         */
        rilEventAddWakeup (socket_listen_p->listen_event);
接口调用说明:
    /* 完成rild 下发的 solicited 类型的请求后，使用该接口上报结果
       RIL_Token t:
        与请求对应，在refril中决定一个response的目标请求
     */
    #define RIL_onRequestComplete(t, e, response, responselen) s_rilenv->OnRequestComplete(t,e, response, responselen)
    /*向rild deamo发送Unsolicited类型的信息*/
    #define RIL_onUnsolicitedResponse(a,b,c) s_rilenv->OnUnsolicitedResponse(a,b,c)
    /*向rild deamo请求执行定时任务*/
    #define RIL_requestTimedCallback(a,b,c) s_rilenv->RequestTimedCallback(a,b,c)
    /**是否为异步操作,目前看来应该是同步的 */
    at_send_command()
###########################################moden基带信息获取分析###############################################
涉及源码:
    hardware/ril/libril/ril_commands.h
    hardware/ril/libril/ril_unsol_commands.h
结论:
    1.最终从rild sokcet接收的请求都调用ril.cpp@processCommandsCallback 来处理
涉及指令:
    logcat -b radio WH-AT:V WH-RIL:V RILD:V RILC:V *:S&
    logcat -b radio -f /sdcard/baseVersion.log -d 
疑问:
    1.RIL.java的数据是如何传到rild的，现在暂时没看到哪里能读到数据rild的数据
    RIL_startEventLoop 
        //创建一个子线程
        1.eventLoop
            A1.s_fdWakeupRead = filedes[0];
            A2.s_fdWakeupWrite = filedes[1];
            A/**建立一个初始化rild_event */
            A3.ril_event_set (&s_wakeupfd_event, s_fdWakeupRead, true,processWakeupCallback, NULL);
            A4.rilEventAddWakeup (&s_wakeupfd_event);
                /**
                 * 1.将该ev挂到watch_table全局的ril_event链表中
                 *  watch_table[i] = ev;
                 * 2.使用select机制监听 pip的读端点，只要 s_fdWakeupWrite写入数据就触发该事件
                    FD_SET(ev->fd, &readFds);
                */
                B1.ril_event_add(&s_wakeupfd_event);
                /**
                 * 1.对于在同一线程中不进行触发processWakeupCallback调用,直接返回
                 */
                B2.triggerEvLoop();
            /**直接进入循环等待 */
            A5.ril_event_loop();//ril_event.cpp
                /**监听挂到rfds的select监控表 */
                B1.n = select(nfds, &rfds, NULL, NULL, ptv);
                /**
                 * 1.循环检查watch_table表中的fd是否有匹配的
                 * 2.如果有就将 ril_event rev加到 pending_list 队列中去
                 *  addToList(rev, &pending_list);
                 */
                B2.processReadReadies()
                /**
                 * 调用 pending_list 中所有的 rild_event->func
                 */
                B3.firePending();
        /**主线程 */
        2.RIL_register(funcs);
            /**
             * static SocketListenParam s_ril_param_socket;
             * ril.cpp
             */
            A1.s_ril_param_socket = {
                RIL_SOCKET_1,             /* socket_id */
                -1,                       /* fdListen */
                -1,                       /* fdCommand */
                PHONE_PROCESS,            /* processName */
                &s_commands_event,        /* commands_event */
                &s_listen_event,          /* listen_event */
                processCommandsCallback,  /* processCommandsCallback */
                NULL,                     /* p_rs */
                RIL_TELEPHONY_SOCKET      /* type */
                };
            A2.startListen(RIL_SOCKET_1, &s_ril_param_socket);
                //根据RIL_SOCKET_1设置socket_name = rild, 最终返回 "rild"
                strncpy(socket_name, RIL_getRilSocketName(), 9);
                fdListen = android_get_control_socket(socket_name);
                ret = listen(fdListen, 4);
                //这里的 socket_listen_p == s_ril_param_socket
                socket_listen_p->fdListen = fdListen;
                /**
                 * 1.设置一个初始化一个ril_event事件
                 *  listen_event->fd = fdListen;
                    listen_event->index = -1;
                    listen_event->persist = false;
                    listen_event->func = listenCallback;
                    listen_event->param = socket_listen_p;
                    fcntl(listen_event->fd, F_SETFL, O_NONBLOCK);
                    2.当 rild socket 的fd有数据后则调用 listenCallback 进行处理
                */
                ril_event_set (socket_listen_p->listen_event, fdListen, false,
                        listenCallback, socket_listen_p);
                /**
                 * 1.把 rild socket句柄添加到readFds中，select监视列表中去
                 *  ril_event_add()
                 *      FD_SET(ev->fd, &readFds);
                 */
                rilEventAddWakeup (socket_listen_p->listen_event);
                
WH-RIL  : onRequest: BASEBAND_VERSION,3,51
WH-AT   : ============AT> AT+CGMR
WH-AT   : ============AT< +CGMR: WH_G405tf_FLASH_V1.0.31B09P23_190227
WH-AT   : ============AT< OK

打印函数分析:
    startResponse;                                      sprintf(printBuf, "%s {", printBuf)
    appendPrintBuf("%s%s", printBuf, (char*)response);  snprintf(printBuf, PRINTBUF_SIZE, x)
    closeResponse;                                      sprintf(printBuf, "%s}", printBuf)、
//下发请求
    RIL_REQUEST_BASEBAND_VERSION
    

######################################################问题解析######################################################
上报已注册网络:

老版本：   
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

*************************************************************Phone应用的初始化****************************************************
参考资料:
    //关于ril
    https://source.android.com/devices/tech/connect/ril
AndroidN对 (Radio Interface Layer)RIL的改进
        1.标准化错误码
        2.
