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