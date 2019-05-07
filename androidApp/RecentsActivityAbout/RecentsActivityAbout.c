近期任务:
ActivityManager: START u0 {flg=0x10804000 cmp=com.android.systemui/.recents.RecentsActivity} from uid 10014
ActivityManager: getPackageFerformanceMode--ComponentInfo{com.android.systemui/com.android.systemui.recents.RecentsActivity}----com.android.systemui
基本概念:
    SystemServicesProxy
        Acts as a shim around the real system services that we need to access data from, and provides a point of injection when testing UI.
        貌似是一个隔离层
    Recents extends SystemUI
        An implementation of the SystemUI recents component, which supports both system and secondary users.
    RecentsPackageMonitor extends PackageMonitor
        跟踪包的状态变化
    //用于存放最近任务图
    mRecentsView = findViewById(R.id.recents_view);
    RecentsTaskLoader:
        最近任务栈加载器
    RecentsTaskLoadPlan：
        RecentsTaskLoader状态的记录表