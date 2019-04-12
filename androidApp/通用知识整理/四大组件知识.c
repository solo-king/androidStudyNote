一、Activity 相关:
    1.如何写一个没有界面的广播接收应用
        2.1隐藏应用在应用列表中的图标
            启动Activity的属性中不添加<category android:name="android.intent.category.LAUNCHER" />
    2.6 app安装后就能相应广播
二、service 相关:
    注意:Service默认运行在主线程当中的。也就是说，我们需要在服务的内部手动创建子线程，并在这里执行具体的任务，否则就有可能出现主线程被阻塞住的情况
        1.如何定义一个Service
                1.1extends Service
                1.2重写 onCreate()、onStartCommand()和 onDestroy()
                        onStartCommand()方法会在每次服务启动的时候调用
                1.3 在AndroidManifest.xml中定义服务
        2.如何运行Service
                Intent startIntent = new Intent(this, MyService.class);
                startService(startIntent); // 启动服务
        3.如何停止Service
                被动停止服务:
                    Intent stopIntent = new Intent(this, MyService.class);
                    stopService(stopIntent); // 停止服务
                自我停止服务:
                        在服务代码中dstopSelf()
        4.活动与Service
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
        4.Service内部实现机制
            Service存在的问题:
                    后台服务系统优先级比较低，在内存不足的情况下会被系统kill
                    前台服务则可以避免此问题
        5.如何跨应用调用API
        6.问题总结
            3.6.1客户端服务绑定服务端注册服务注意事项
                服务端AndroidManifest.xml中注册
                    <service android:name=".Services.MyService2"
                        android:exported="true"
                        android:enabled="true">
                        <intent-filter>
                            <action android:name="cn.usr.www.usrbroadcastcentor.Services.MyService2"></action>//随意不重复即可
                            <category android:name="android.intent.category.DEFAULT"></category>//
                        </intent-filter>
                    </service>
        7.相关问题
            7.1 Failure [INSTALL_FAILED_INVALID_APK: Package couldn is not  be installed in /data/app/cn.usr.www.usrbroadcastcentor-1: Package /data/app/cn.usr.www.usrbroadcastcentor-1/base.apk code is missing]
                解决办法java的源码目录结构必须在src下
                IntentService 类为异步的执行且会自动停止的服务
三、Broadcast 相关:
    1.如何注册一个广播接收器
        1.1静态注册
            public class MyBroadcastReceiver extends BroadcastReceiver {
                    @Override
                    public void onReceive(Context context, Intent intent) {
                    Toast.makeText(context, "received in MyBroadcastReceiver",
                    Toast.LENGTH_SHORT).show();
                    }
            }
            <receiver android:name=".MyBroadcastReceiver">
                    <intent-filter>
                    <action android:name="com.example.broadcasttest. MY_BROADCAST"/>
                    </intent-filter>
            </receiver>
        1.2动态注册
            
        1.3如何发送自定义广播
            1.3.1 发送标准广播
                Intent intent = new Intent("com.example.broadcasttest.MY_BROADCAST");
                sendBroadcast(intent);
            1.3.2 发送有序广播
                Intent intent = new Intent("com.example.broadcasttest.MY_BROADCAST");
                sendOrderedBroadcast(intent, null);
        1.4 广播接收器如何接收广播发送者的参数
            发送者使用 Intent 进行传递的，因此你还可以在 Intent 中携带一些数据传递给广播接收器
            https://www.jianshu.com/p/a8e43ad5d7d2
        1.5 广播接收器如何发送返回值给广播发送者
            sendOrderedBroadcast
四、provider 相关:
