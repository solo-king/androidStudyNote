参考资料:
    //概览版本的官方资料
    https://developer.android.com/reference/android/app/Service
    //详细的官方资料
    https://developer.android.com/guide/components/services
    //Remote Messenger Service Sample
    https://developer.android.com/reference/android/app/Service#LocalServiceSample
使用注意事项:
    1.Service默认运行在主线程当中的。也就是说，我们需要在服务的内部手动创建子线程，并在这里执行具体的任务，否则就有可能出现主线程被阻塞住的情况
    2.后台服务系统优先级比较低，在内存不足的情况下会被系统kill但前台服务则可以避免此问题
    3.如何让Service在后台不被杀掉

定义一个Service
    1.1extends Service
    1.2重写 onCreate()、onStartCommand()和 onDestroy()
            onStartCommand()方法会在每次服务启动的时候调用
    1.3 在AndroidManifest.xml中定义服务
让Service运行
    Intent startIntent = new Intent(this, MyService.class);
    startService(startIntent); // 启动服务
停止Service
    被动停止服务:
        Intent stopIntent = new Intent(this, MyService.class);
        stopService(stopIntent); // 停止服务
    自我停止服务:
            在服务代码中dstopSelf()
活动与Service
    绑定:
        private MyService.DownloadBinder downloadBinder;
        private ServiceConnection connection = new ServiceConnection() {
                @Override
                public void onServiceDisconnected(ComponentName name) {
                }
                @Override
                public void onServiceConnected(ComponentName name, IBinder service) {
                    downloadBinder = (MyService.DownloadBinder) service;
                    downloadBinder.startDownload();
                    downloadBinder.getProgress();
                }
        };
        
        Intent bindIntent = new Intent(this, MyService.class);
        bindService(bindIntent, connection, BIND_AUTO_CREATE); // 绑定服务
        注意:
            从ServiceConnection中获得的服务不是那么及时，所以要使用时需要做一个延迟
    解绑:
            unbindService()
7.相关问题
    7.1 Failure [INSTALL_FAILED_INVALID_APK: Package couldn is not  be installed in /data/app/cn.usr.www.usrbroadcastcentor-1: Package /data/app/cn.usr.www.usrbroadcastcentor-1/base.apk code is missing]
        解决办法java的源码目录结构必须在src下
        IntentService类为异步的执行且会自动停止的服务

IntentService
    特性:
        1.IntentService is a base class for Services that handle asynchronous requests (expressed as Intents) on demand.
        2.The service is started as needed, handles each Intent in turn using a worker thread, and stops itself when it runs out of work