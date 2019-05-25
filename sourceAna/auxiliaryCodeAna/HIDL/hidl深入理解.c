参考资料:
    //官方HIDL资料
    https://source.android.com/devices/architecture/hidl
    //经过android修改的 lts版本kernel
    https://android.googlesource.com/kernel/common/
    //HIDL语法风格
    https://source.android.com/devices/architecture/hidl/code-style.html
工具说明:
    //用于更新 hardware/interfaces/下所有hal.l的Android.mk Android.bp
    ./hardware/interfaces/update-makefiles.sh
    创建hdl server
        https://source.android.com/devices/architecture/hidl-cpp#creating-the-hal-server
        关键命令(位于Android Root目录下):
            1.先在hardwara/interfaces/1.0/xxx下创建xxx.hal
            2.执行 ./hardware/interfaces/update-makefiles.sh
            3.创建实现
                PACKAGE=android.hardware.xxx@1.0
                LOC=hardware/interfaces/xxx/1.0/default/
                m -j hidl-gen
                hidl-gen -o $LOC -Lc++-impl -randroid.hardware:hardware/interfaces \
                    -randroid.hidl:system/libhidl/transport $PACKAGE
                hidl-gen -o $LOC -Landroidbp-impl -randroid.hardware:hardware/interfaces \
                    -randroid.hidl:system/libhidl/transport $PACKAGE
            4.最终生成目录结构
                .
                ├── 1.0
                │   ├── Android.bp
                │   ├── Android.mk
                │   ├── default
                │   │   ├── Android.bp
                │   │   ├── Mycookie.cpp
                │   │   └── Mycookie.h
                │   └── IMycookie.hal
                └── Android.bp
HIDL简述:
    1.intended to be used for inter-process communication (IPC)
    2. For libraries that must be linked to a process,passthrough mode is also available (not supported in Java).
    3.The syntax of HIDL will look familiar to C++ and Java programmers, though with a different set of keywords
    4.HIDL也使用Java-style的注释
HIDL设计意图:
    1.当ASOP更新时不需要重新编译HALs
    2.HALs will be built by vendors or SOC makers and put in a /vendor partition on the device,
    3.版本升级时只需要替换vendor中的HALs而不需要重新编译HALs
    4.主要优点
        Interoperability(互通性,暂时理解为高兼容)：
            a.可支持各种架构下的工具链及编译配置
            b.HIDL一经发布则不允许更改
        Efficiency(高效的):
            a.支持两种模式调用
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
使用binder化的HIDL
    参考代码:
        hardware/interfaces/usb/1.0
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
    10.现在第三方的service使用 vndservicemanager 来管理，servicemanager则专用于framework和app之间的通讯使用
    11.
老版本的HALs转化工具
    make c2hal -j8
    错误：
        Assertion `cnt < (sizeof (_nl_value_type_LC_TIME) / sizeof (_nl_value_type_LC_TIME[0])) failed
        解决办法:
            export LC_ALL=C

#########################################c++版本HIDL#############################
参考资料：
    https://source.android.com/devices/architecture/hidl-cpp
概念说明:
    hidl-gen
    aidl-cpp
需要解决的问题:
    1.接口必须稳定
        HIDL replaces these HAL interfaces with stable, versioned interfaces, which can be client- and server-side HIDL interfaces in C++ (described below) or Java
    2.hidl-gen 编译器编译 .hal文件时
        2.1 生成的详细中间文件
        2.2 中间文件如何被打包
        2.3 如何被集成到c++代码中，并被其使用的

light目录结构:
    /hardware/interfaces/light$ tree
    .
    ├── 2.0
    │   ├── Android.bp
    │   ├── Android.mk
    │   ├── default
    │   │   ├── android.hardware.light@2.0-service.rc
    │   │   ├── Android.mk
    │   │   ├── Light.cpp
    │   │   ├── Light.h
    │   │   └── service.cpp
    │   ├── ILight.hal
    │   ├── types.hal
    │   └── vts
    │       ├── Android.mk
    │       └── functional
    │           ├── Android.bp
    │           └── VtsHalLightV2_0TargetTest.cpp
    └── Android.bp
hdil接口实现:
    // From the IFoo.h header
    using android::hardware::samples::V1_0::IFoo;

    class FooImpl : public IFoo {
        Return<void> someMethod(foo my_foo, someMethod_cb _cb) {
            vec<uint32_t> return_data;
            // Compute return_data
            _cb(return_data);
            return Void();
        }
        ...
    };
暴露hdil实现的service:
    1.Register the interface implementation with the hwservicemanager (see details below)
        1.1 使用 IFoo.h 中的 registerAsService()方法注册一个IFoo.h实现到 hwservicemanager
        1.2 服务端使用名字将自己注册到 hwservicemanager中，client则通过
            ::android::sp<IFoo> myFoo = new FooImpl();
            ::android::sp<IFoo> mySecondFoo = new FooAnotherImpl();
            status_t status = myFoo->registerAsService();
            status_t anotherStatus = mySecondFoo->registerAsService("another_foo");
    2.Pass the interface implementation as an argument of an interface method (for detals, see Asynchronous callbacks).

包定义(package):
    决定一个hidl的唯一性
interface(接口定义):

数据类型(data type):
    hidl   <-> c++
方法/函数(function):
    返回值(含如下三种方式):
        https://source.android.com/devices/architecture/hidl-cpp/functions#return-by
        1.返回一个值与c++中类似.在 .hal中使用 generates
            .hal:
                someMethod(Foo foo) generates (int32_t ret);
            c++:
                Return<int32_t> someMethod(Foo foo) generates (int32_t ret){}
        2.返回多个参数则使用回调的方式
            .hal:
                register_callback(IFooCallback callback);
            c++:
                Return<void> register_callback(IFooCallback callback){}
        3.不写 generates则在c++中默认为Return<void>
将hdil挂到系统中启动
    1.接口system -->hidl
    2.rc-->init进程去启动
    3.seAndroid 规则-->保证init能启动该服务
    4.c++ client -->调用
        USER           PID    PPID      VSZ     RSS         WCHAN                   ADDR    S   NAME
        system         243     1        12136   4260    binder_thread_read      7278a49560  S   android.hardware.light@2.0-service
HIDL基本数据类型:
    1.enum 
    2.bitfield types (which always derive from primitive types), map to standard C++ types such as std::uint32_t from cstdint.
    3.
c++使用实现:

    Using passthrough mode：Binderizing passthrough HAL:
        vendor:
            ./lib64/hw/lights.rk3399.so
            ./lib64/hw/android.hardware.light@2.0-impl.so
            ./bin/hw/android.hardware.light@2.0-service
        system:
            ./lib64/android.hardware.light@2.0.so
            ./lib64/android.hardware.tests.extension.light@2.0.so
        涉及指令:
            logcat 'Lights Hal':V MyLight:V light:V  *:S&
        问题探讨:
            1.getSupportedTypes() generates (vec<Type> types);是如何转变为支持函数回调的
                //hardware/interfaces/light/2.0/ILight.hal
                getSupportedTypes() generates (vec<Type> types);
                //out/soong/.intermediates/hardware/interfaces/light/2.0/android.hardware.light@2.0_genc++_headers/gen/android/hardware/light/2.0/ILight.h
                using getSupportedTypes_cb = std::function<void(const ::android::hardware::hidl_vec<Type>& types)>;
                virtual ::android::hardware::Return<void> getSupportedTypes(getSupportedTypes_cb _hidl_cb) = 0;
                //hardware/interfaces/light/2.0/default/Light.h
                Return<void> getSupportedTypes(getSupportedTypes_cb _hidl_cb)  override;
                暂定原因:
                    https://source.android.com/devices/architecture/hidl/services
                    If only one value is returned and it is a primitive type, a callback is not used and the value is returned from the method. 
                    The server implements the HIDL methods and the client implements the callbacks.
hardware/interfaces/mylight:
    //编译生成的中间产物
    ./soong/.intermediates/hardware/interfaces/mylight/1.0/android.hardware.mylight@1.0_genc++_headers/gen/android/hardware/mylight/1.0/IMylight.h
    ./target/common/gen/JAVA_LIBRARIES/android.hardware.mylight-V1.0-java-static_intermediates/android/hardware/mylight/V1_0/IMylight.java
    ./target/common/gen/JAVA_LIBRARIES/android.hardware.mylight-V1.0-java_intermediates/android/hardware/mylight/V1_0/IMylight.java

    using getSupportedTypes_cb = std::function<void(const ::android::hardware::hidl_vec<LightType>& types)>;
    virtual ::android::hardware::Return<void> getSupportedTypes(getSupportedTypes_cb _hidl_cb) = 0;
seAndroid:
    参考资料:
        //对已定义的宏功能说明
        https://android.googlesource.com/platform/system/sepolicy/+/master/public/te_macros
    //参考文件:/system/vendor/hal_light_default.te
    #集成自domain
    type hal_light_default, domain;
    //从 hal_light domain中集成所有的权限
    hal_server_domain(hal_light_default, hal_light)
    //定义一个type
    type hal_light_default_exec, exec_type, vendor_file_type, file_type;
    //允许init启动的一个进程
    init_daemon_domain(hal_light_default)

Configstore HAL:
    作用:
        1.存储system/vendor之间的共享属性.
        2.减少

########################################################java HIDL################################################################
目的:
    1.detail how to create, register, and use services
    2.explain how HALs and HAL clients written in Java interact with the HIDL RPC system
与c++ 版本的hdil区别:
    1.The Java HIDL backend does NOT support union types, native handles, shared memory, and fmq types.