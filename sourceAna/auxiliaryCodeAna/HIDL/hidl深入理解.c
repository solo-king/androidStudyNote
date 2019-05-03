参考资料:
    //官方HIDL资料
    https://source.android.com/devices/architecture/hidl
    //经过android修改的 lts版本kernel
    https://android.googlesource.com/kernel/common/
    //HIDL语法风格
    https://source.android.com/devices/architecture/hidl/code-style.html

HIDL简述:
    1.intended to be used for inter-process communication (IPC)
    2. For libraries that must be linked to a process,passthrough mode is also available (not supported in Java).
    3.The syntax of HIDL will look familiar to C++ and Java programmers, though with a different set of keywords
    4.HIDL也使用Java-style的注释
HIDL设计意图:
    1.单ASOP更新时不需要重新编译HALs
    2.HALs will be built by vendors or SOC makers and put in a /vendor partition on the device,
    3.版本升级时只需要替换vendor中的HALs而不需要重新编译HALs
    4.主要优点
        Interoperability(互通性,暂时理解为高兼容)：
            a.可支持各种架构下的工具链及编译配置
            b.HIDL一经发布则不允许更改
        Efficiency(高效的):
            a.支持两种模式调用
            b.
        Intuitive(直观的):
            a.参数调用只允许类型为in
            b.values that cannot be efficiently returned from methods are returned via callback functions
            c.Data needs to persist only for the duration of the called function and may be destroyed immediately after the called function returns
老版本的HAL架构:    
    1.Android 8.0开始将不被支持
    2.将被更新更加模块化的架构替代(HIDL)
HAL的类型:
    Binder化的HALs(Binderized HALs):
        1.这种方式替代老式的HALs
        2.Android framework and HALs communicate with each other using binder inter-process communication (IPC) calls. 
        3.Android 8.0 以及之后的版本仅仅支持binderized HALs.
    Passthrough HALs：
        1.用于兼容Android 8之前的HALs
        2.A HIDL-wrapped conventional or legacy HAL. These HALs wrap existing HALs and can serve the HAL in binderized and same-process (passthrough) modes. Devices upgrading to Android 8.0 can use passthrough HALs.
    Same-Process HALs(SP-HALs)：
        1.在总是在同一进程的情况下使用
        2.They include all HALs not expressed in HIDL as well as some that are not binderized
        3.Membership in the SP-HAL set is controlled only by Google, with no exceptions.
    Conventional：
        1.在Android O之后被弃用
        2.The bulk of Android system interfaces (camera, audio, sensors, etc.)
        3.这些接口被定义在hardware/libhardware/include/hardware
    legacy HALs:
        1.在Android O之后被弃用
        2. A few important subsystems (Wi-Fi, Radio Interface Layer, and Bluetooth) are legacy HALs. 
        3.代码位于hardware/libhardware/libhardware_legacy中
    Dynamically Available HALs:
        1.从Android p开始支持
        2. In Android 9, init includes three additional control messages (e.g. ctl.start): ctl.interface_start, ctl.interface_stop, and ctl.interface_restart
        3.处理的应用场景：
            Android 9 supports the dynamic shutdown of Android hardware subsystems when they are not in use or not needed.
            For example, when a user is not using Wi-Fi, the Wi-Fi subsystems should not be taking up memory, power, or other system resources.
        4.实现原理:
            1.通过hwservicemanager给init进程发送指令
使用passthrough mode包装传统的HALs

HIDL语法
    
    /** */:
        indicates a documentation comment. These can be applied only to type, method, field, and enum value declarations.
    /* */:
        indicates a multiline comment.
    //:
        indicates a comment to end of line. Aside from //, newlines are the same as any other whitespace.
        In the example grammar below, text from // to the end of the line is not part of the grammar but is instead a comment on the grammar.
    [empty]:
        代表empty可能为空
    ?:
        following a literal or term means it is optional.
    ... indicates sequence containing zero or more items with separating punctuation as indicated. There are no variadic arguments in HIDL.
    Commas(逗号):
         分隔元素
    Semicolons(分号):
         terminate each element, including the last element.
    UPPERCASE is a nonterminal.
    italics(斜体):
        is a token family such as integer or identifier (standard C parsing rules).
    constexpr:
         is a C style constant expression (such as 1 + 1 and 1L << 3).
    import_name is a package or interface name, qualified as described in HIDL Versioning.
    Lowercase words are literal tokens.
接口和包:
    1.Package names can have sublevels such as package.subpackage
    2.The root directory for published HIDL packages is hardware/interfaces or vendor/vendorName (e.g. vendor/google for Pixel devices)
    3.The package name forms one or more subdirectories under the root directory; all files defining a package are in the same directory. 
        For example, package android.hardware.example.extension.light@2.0 could be found under hardware/interfaces/example/extension/light/2.0
    4.下面接口已经隐士导入
        ping
        interfaceChain
        interfaceDescriptor
        notifySyspropsChanged
        linkToDeath
        unlinkToDeath
        setHALInstrumentation
        getDebugInfo
        debug
        getHashChain
服务和数据传输(Services & Data Transfer):
    目的:
        1.如何注册一个服务
        2.如何找到一个已经注册的服务
        3.如何发送数据到一个已经注册的服务中
    服务的注册:
        1.注册的服务名字不需要和接口或者包名有关系(The registered name need not be related to the interface or package name.)
        2.如果注册服务时没有指定名称，将使用"default"用作该服务的名称
        3.注册代码:
            status_t status = myFoo->registerAsService();
            status_t anotherStatus = anotherFoo->registerAsService("another_foo_service");  // if needed
        4.hidl会自动分配版本号，不需要手动设置。并且用户可以通过如下方法获得当前hidl版本
            android::hardware::IInterface::getInterfaceVersion()
服务的死亡通知(Service death notifications):
    客户端想接收服务的死亡通知必须符合一下几点
        1.Subclass the HIDL class/interface hidl_death_recipient (in C++ code, not in HIDL).
        2.Override its serviceDied() method.
        3.Instantiate an object of the hidl_death_recipient subclass. 
        4.Call the linkToDeath() method on the service to monitor, 
            passing in the IDeathRecipient s interface object. Note that this method does not take ownership of the death recipient or the proxy on which it is called.
            参考代码如下:
                class IMyDeathReceiver : hidl_death_recipient {
                    virtual void serviceDied(uint64_t cookie,
                                            wp<IBase>& service) override {
                        log("RIP service %d!", cookie);  // Cookie should be 42
                    }
                };
                ....
                IMyDeathReceiver deathReceiver = new IMyDeathReceiver();
                m_importantService->linkToDeath(deathReceiver, 42);
        
Fast Message Queue (FMQ)
    解决问题点:
        1.不涉及进程间的调度
        2.为了减少开销，不需要kernel参与
    基本概念:
        1.supported only in C++
        2.
Android O开始binder的变化:
    1.Multiple binder domains 
    2.Scatter-gather，减少3倍的拷贝次数
        Android 8 uses scatter-gather optimization to reduce the number of copies from 3 to 1
            In previous releases of Android, every piece of data in a binder call was copied three times:
            Once to serialize it into a Parcel in the calling process
            Once in the kernel driver to copy the Parcel to the target process
            Once to unserialize the Parcel in the target process
    3.锁粒度的改变
        官方资料：
        https://source.android.com/devices/architecture/hidl/binder-ipc#locking
    4.Real-time priority inheritance
    5.Userspace changes
    6./dev/binder节点为framework专用,厂商的 将无权使用/dev/binder
    7.厂商进程如果想使用必须试下你HIDL，其binder节点为/dev/hwbinder
    8./dev/下binder节点用途说明
        /dev/binder 	IPC between framework/app processes with AIDL interfaces
        /dev/hwbinder 	IPC between framework/vendor processes with HIDL interfaces
                        IPC between vendor processes with HIDL interfaces
        /dev/vndbinder 	IPC between vendor/vendor processes with AIDL Interfaces
    9.使用 /dev/vndbinder时需要在所有代码前调用如下代码
        ProcessState::initWithDriver("/dev/vndbinder");
    10.现在第三方的service使用 vndservicemanager来管理，servicemanager则专用于framework和app之间的通讯使用
    11.
老版本的HALs转化工具
    make c2hal -j8
    错误：
        Assertion `cnt < (sizeof (_nl_value_type_LC_TIME) / sizeof (_nl_value_type_LC_TIME[0])) failed
        解决办法:
            export LC_ALL=C
    

    
