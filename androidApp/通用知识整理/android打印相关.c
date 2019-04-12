public class NetworkManagementService extends INetworkManagementService.Stub
        implements Watchdog.Monitor {
    private static final String TAG = "NetworkManagement";
    private static final boolean DBG = Log.isLoggable(TAG, Log.DEBUG);
}
说明:
    1.Log.isLoggable(TAG, Log.DEBUG);
        对于消息等级大于Log.DEBUG的才输出.
    2.动态更改代码中 Log.isLoggable(TAG, Log.DEBUG);打印log的等级
        2.1 vi /data/local.prop
            log.tag.NetworkManagement=VERBOSE //对于此处要打印所有信息时
        2.2 chmod 644 /data/local.prop
        2.3 reboot