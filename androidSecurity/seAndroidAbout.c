参考资料:
    //官方资料
    https://source.android.com/security/selinux
    //selinux相关的权威参考笔记
    https://source.android.com/security/selinux#supporting_documentation
    //官方对于一些类可用的控制权限
    https://source.android.com/security/selinux/customize#available_controls
SELinux 设计哲学:
    1.任何没有明确指明的权限都应该被拒绝
        Anything not explicitly allowed is denied
    2.

在Android 8.0上的变化:
    1.更新和Treble项目的合作-->貌似指代在vendor代码中可以定义domain
    2.allow device manufacturers and SOC vendors to update their parts of the policy
    3.
可能存在的问题:
    1.framework和vendor中使用的seAndroid机制不一致导致的兼容性问题
        解决办法:
            https://source.android.com/security/selinux/compatibility
基本概念:
    discretionary access control (DAC):
    mandatory access control (MAC):
    Domain:
        1.一个进程或者一系列进程的标签，也叫作域domain用来标志进程的类型
    Type:
        A label for the object (e.g. file, socket) or set of objects.
    Class
        被访问对象(客体)所属的类型
        The kind of object (e.g. file, socket) being accessed.
    Permission
        The operation (e.g. read, write) being performed.
    一些通用的定义:
        1.These macros files are located in the system/sepolicy directory
        2.In Android 8.0 and higher, 这些通用的宏定义位于system/sepolicy/public
    通用规则:
        RULE_VARIANT SOURCE_TYPES TARGET_TYPES : CLASSES PERMISSIONS
        注释:
            The rule indicates what should happen when a subject labeled with 
            any of the source_types attempts an action corresponding to any of the permissions 
            on an object with any of the class classes that has any of the target_types label. 
            The most common example of one of these rules is an allow rule,
    不允许直接范文以下通用标签:
        socket_device
        device
        block_device
        default_service
        system_data_file
        tmpfs
应用 Selinux for Android
    1.不要直接修改 system/sepolicy下的策略
    2.在android 8.0之前的版本，添加自定义的策略应该在/device/manufacturer/device-name/sepolicy中
    3.在 android 8 之后在the changes you make to these files should only affect policy in your vendor directory.
    4.Android O之后策略的目录
        system/sepolicy/private
        system/sepolicy/public
        system/sepolicy/vendor
        device/manufacturer/device-name/sepolicy
    5.如何将策略编译进固件中
        Build and flash the boot and system images.
    


