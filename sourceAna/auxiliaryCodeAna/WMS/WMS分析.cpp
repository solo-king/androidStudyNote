宏观功能:   
    全局窗口管理

    全局的事件管理派发
微观功能:
    支持Binder通信,SurfaceFlinger、AMS、其他应用进程与其的通讯方式
    派发IMS发送过来的功能
    根据AMS的变化，从界面上将其表现出来
    对窗口的添加或删除
    启动窗口支持
    窗口动画
    窗口层级
    WMS访问APP
疑问:
    为什么说在IWindowManger::openSession()中提交请求，可以减轻WMS的负担
    WindowManagerImp(含Context.DISPLAT_SERVICE)、ViewRoot代表什么
    Session.java中的mService代表什么
    Decor:
        代表Activity整棵树的最外围
    java层的Surface与c++层的Surface区别，java层的Surface与WMS的关系
WMS的工作方式:
    事件投递：
        1.通过IwindowManager.xxx()-->wmHandlerThread
        2.直接调用
关系:
    SurfaceFlinger与WMS之间的关系

    WMS、AMS与Activity之间的关系
        在WMS内部使用WindowState表示一个Activity
        在WMS内部使用AppWindowToken表示AMS中的ActivityRecord
        在AMS中使用ActivityRecord记录一个Activity
    ViewRootImpl:
        管理一颗View树
    
窗口的添加:
    WindowManagerImpl.java-->addView@WindowManagerGlobal.java-->addView@ViewRootImpl.java-->setView
    最终由WindowManagerGlobal.java-->addView处理
应用启动窗口过程:
    ActivityStack.java-->startActivityLocled@WMS-->addAppToken
    

