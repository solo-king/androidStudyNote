//邓平凡书本阅读
AcitvityThread:
    1.应用进程的主线程.
    2.调度及执行在形成中运行的四大组件
    3.里面可以包含多个Application
AThread:
    1.AMS的工作线程
应用进程:
    运行APK的进程,由Zyote派生(fork),其上面运行着虚拟机
系统进程
Application:
    1.一个进程支持多个Application
    2.内部包含四大组件
IApplicatonThread接口:
    1.AMS与应用程序通讯的接口
    2.由AMS内部类实现,ApplicationThreadProxy
ActivityStack:
    1.管理一堆的活动(State and management of a single stack of activities.)
    2.其中的ArryList<ActivityRecord> mHistory记录所有的ActivityRecord
ActivityRecord:
    代表一个Activity
TaskRecord:
    1.代表一个Task
    1.一个Task中管理多个Acivity,但其中的Activity是为了一个事件服务的,例如发邮件由选择联系人活动 邮件正文编写活动 发送邮件活动等组成
ActivityInfo:
    Information you can retrieve about a particular application activity or receiver. This corresponds to information collected
    from the AndroidManifest.xml s ;activity; and receiver.

launch mode:
    1.Activity 的启动模式,含
        standard:
            一个Task中可以有多个相同类型的Activity,即可以有同一个类的多个对象
         singleTop:
         singleTask:
            对应的Activity只存在一个实例,并且和Task绑定,与singleInstance不同之处在于此Activity所在的Task还可以拥有其他类型的Activity.
        singleInstance

    2.目的用于控制Activity 和Task之间的关系
am启动Activity分析:
    本次命令:
        am start -W -n com.dfp.test/.TestActivity
    am最终调用AMS的startActivityAndWait()处理


