关键术语和概念

Here is a summary of key BLE terms and concepts:

Generic Attribute Profile (GATT):
    The GATT profile is a general specification for sending and receiving short pieces of data known as "attributes" over a BLE link.
    All current Low Energy application profiles are based on GATT. The Bluetooth SIG defines many profiles for Low Energy devices. 
    A profile is a specification for how a device works in a particular application.
    Note:
        a device can implement more than one profile. 
        For example, a device could contain a heart rate monitor and a battery level detector.
Attribute Protocol (ATT):
    GATT is built on top of the Attribute Protocol (ATT). This is also referred to as GATT/ATT.
    ATT is optimized to run on BLE devices. To this end, it uses as few bytes as possible. 
    Each attribute is uniquely identified by a Universally Unique Identifier (UUID), 
    which is a standardized 128-bit format for a string ID used to uniquely identify information. 
    The attributes transported by ATT are formatted as characteristics and services.
Characteristic:
    A characteristic contains a single value and 0-n descriptors that describe the characteristic s value. 
    A characteristic can be thought of as a type, analogous to a class. 
Descriptor:
    Descriptors are defined attributes that describe a characteristic value. 
    For example, a descriptor might specify a human-readable description, an acceptable range for a characteristic s value, 
    or a unit of measure that is specific to a characteristic s value.
Service:
    A service is a collection of characteristics. 
    For example, you could have a service called "Heart Rate Monitor" that includes characteristics such as "heart rate measurement." 
    You can find a list of existing GATT-based profiles and services on bluetooth.org.

参考资料:
    //官方资料
    https://developer.android.com/guide/topics/connectivity/bluetooth

Using the Bluetooth APIs, an Android application can perform the following:
    Scan for other Bluetooth devices
    Query the local Bluetooth adapter for paired Bluetooth devices
    Establish RFCOMM channels
    Connect to other devices through service discovery
    Transfer data to and from other devices
    Manage multiple connections
基本概念：
    设备模式:
        discoverable device
    服务模式:
        service discovery process
涉及权限:
    <uses-permission android:name="android.permission.BLUETOOTH" />
    // 用于初始化及控制蓝牙设备(initiate device discovery or manipulate Bluetooth settings)
    <uses-permission android:name="android.permission.BLUETOOTH_ADMIN" />
    //由于可以通过蓝牙获取定位信息(Bluetooth beacons)，所以需要该权限
    <uses-permission android:name="android.permission.ACCESS_COARSE_LOCATION" />
配置:
    能够方便的链接各种外设
建立通用连接:
    Note:
        先开启蓝牙开关
    1.获取Get the BluetoothAdapter. 
        BluetoothAdapter bluetoothAdapter = BluetoothAdapter.getDefaultAdapter();
        if (bluetoothAdapter == null) {
            // Device doesn't support Bluetooth
        }
    2.使能Enable Bluetooth. 
        if (!bluetoothAdapter.isEnabled()) {
            Intent enableBtIntent = new Intent(BluetoothAdapter.ACTION_REQUEST_ENABLE);
            //REQUEST_ENABLE_BT代表权限请求码
            //权限申请完成结果将回调Activity中的onActivityResult(),并且返回请求码 REQUEST_ENABLE_BT
            startActivityForResult(enableBtIntent, REQUEST_ENABLE_BT);
            //If enabling Bluetooth succeeds, your activity receives the RESULT_OK result code in the onActivityResult() callback. 
            //If Bluetooth was not enabled due to an error (or the user responded "No") then the result code is RESULT_CANCELED.
        }
    3.可以通过注册广播关注蓝牙状态变化
        Optionally, your application can also listen for the ACTION_STATE_CHANGED broadcast intent, 
        which the system broadcasts whenever the Bluetooth state changes. 
        This broadcast contains the extra fields EXTRA_STATE and EXTRA_PREVIOUS_STATE, containing the new and old Bluetooth states, respectively. 
        Possible values for these extra fields are STATE_TURNING_ON, STATE_ON, STATE_TURNING_OFF, and STATE_OFF. 
        Listening for this broadcast can be useful if your app needs to detect runtime changes made to the Bluetooth state.
寻找外设:
    前提:
        1.先蓝牙设备先互配对
        2.再建立链接
    获取已经配对的设备
        Set<BluetoothDevice> pairedDevices = bluetoothAdapter.getBondedDevices();
        if (pairedDevices.size() > 0) {
            // There are paired devices. Get the name and address of each paired device.
            for (BluetoothDevice device : pairedDevices) {
                String deviceName = device.getName();
                String deviceHardwareAddress = device.getAddress(); // MAC address
            }
        }
    注意:
        1. 执行startDiscovery()会占用蓝牙适配器大量的性能资源
        2. 在找到一个设备要链接的设备后，就应该使用 cancelDiscovery()停止继续扫描。
        3. 将要建立设备链接时先使用 cancelDiscovery() 停止扫描
        3. 不要在建立链接的过程中调用 cancelDiscovery()， 否则会直接影响蓝牙的链接信号及带宽
扫描设备:
    调用接口
        startDiscovery()
    获得扫描到的设备信息:
        1. must register a BroadcastReceiver for the ACTION_FOUND intent. 
        2. The system broadcasts this intent for each device. 
        3. The intent contains the extra fields EXTRA_DEVICE and EXTRA_CLASS, which in turn contain a BluetoothDevice and a BluetoothClass, respectively. 
    示例代码:
        @Override
        protected void onCreate(Bundle savedInstanceState) {
            ...

            // Register for broadcasts when a device is discovered.
            IntentFilter filter = new IntentFilter(BluetoothDevice.ACTION_FOUND);
            registerReceiver(receiver, filter);
        }

        // Create a BroadcastReceiver for ACTION_FOUND.
        private final BroadcastReceiver receiver = new BroadcastReceiver() {
            public void onReceive(Context context, Intent intent) {
                String action = intent.getAction();
                if (BluetoothDevice.ACTION_FOUND.equals(action)) {
                    // Discovery has found a device. Get the BluetoothDevice
                    // object and its info from the Intent.
                    BluetoothDevice device = intent.getParcelableExtra(BluetoothDevice.EXTRA_DEVICE);
                    String deviceName = device.getName();
                    String deviceHardwareAddress = device.getAddress(); // MAC address
                }
            }
        };

        @Override
        protected void onDestroy() {
            super.onDestroy();
            ...

            // Don't forget to unregister the ACTION_FOUND receiver.
            unregisterReceiver(receiver);
        }
运行时权限请求:
    参考资料:
        //官方资料
        https://developer.android.com/training/permissions/requesting.html
    //扫描代码
    2019-04-14 19:48:23.933 3395-3395/cn.usr.www.bluetoothtest D/MainActivity: onLeScan:[MAC]0D:06:11:15:1D:C4[NAME]X3C01-1DC4[class]1f00[uuids]null[type]2[boundState]10
    2019-04-14 19:48:23.933 3395-3395/cn.usr.www.bluetoothtest D/chenqigan: getScanRecordUUID:[id]0000f001-0000-1000-8000-00805f9b34f
                                                                                                  0000f000-0000-1000-8000-00805f9b34fb
BLE GATT使用:
    //官方参考资料
    https://developer.android.com/guide/topics/connectivity/bluetooth-le#java
概念:
    Services:
    Characteristics:
监听
