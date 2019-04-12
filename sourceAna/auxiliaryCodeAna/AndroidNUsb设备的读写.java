需要解决问题:
    普通应用能够对USB、SD卡进行文件/文件夹的创建。
涉及源码:
    UsbManager.java 不用多说，对应的是Usb的管理类
    UsbDevice.java Usb设备绑定上，肯定要拿到对应的设备吧
    UsbInterface.java Usb对应的接口，通过接口拿到内部匹配Usbpoint
    UsbEndPoint.java Usb通信数据的传输主要其实就是通过这个类来进行的
    UsbDeviceConnection Usb连接器
官方参考资料:
    //android中对usb设备操作的一个概览
    https://developer.android.google.cn/guide/topics/connectivity/usb/

USB accessory and USB host
    USB accessory模式应用场景：
        robotics controllers; 
        docking stations; 
        diagnostic and musical equipment; 
        kiosks; 
        card readers
最终解决办法(Android 7.1验证通过):
    diff --git a/frameworks/base/services/core/java/com/android/server/pm/PackageManagerService.java b/frameworks/base/services/core/java/com/android/server/pm/PackageManagerService.java
    index 9356dcc..4c132e6 100755
    --- a/frameworks/base/services/core/java/com/android/server/pm/PackageManagerService.java
    +++ b/frameworks/base/services/core/java/com/android/server/pm/PackageManagerService.java
    @@ -10179,7 +10179,8 @@ public class PackageManagerService extends IPackageManager.Stub {
                    case PermissionInfo.PROTECTION_SIGNATURE: {
                        // For all apps signature permissions are install time ones.
                        allowedSig = grantSignaturePermission(perm, pkg, bp, origPermissions);
    -                    if (allowedSig) {
    +                    //对所有应用放开android.permission.WRITE_MEDIA_STORAGE权限.add by chenqigan
    +                    if (allowedSig||perm.equals("android.permission.WRITE_MEDIA_STORAGE")) {
                            grant = GRANT_INSTALL;
                        }
                    } break;

