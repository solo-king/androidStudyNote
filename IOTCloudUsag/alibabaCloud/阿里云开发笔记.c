官方参考资料:
    https://help.aliyun.com/document_detail/30523.html?spm=a2c4g.11186623.6.543.5f813f69LcxkXE
    //设备接入SDK
    https://help.aliyun.com/product/93051.html?spm=a2c4g.11186623.2.15.43f41996aznkYc
解决问题:
    1.系统结构,一些名词概念
        IoT Hub:
            设备与云端安全通信的数据通道.IoT Hub支持PUB/SUB与RRPC两种通信方式，其中PUB/SUB是基于Topic进行的消息路由
            支持多种设备接入协议：支持设备使用CoAP、MQTT、HTTPS协议接入物联网平台
        产品
         	设备的集合，通常指一组具有相同功能的设备。物联网平台为每个产品颁发全局唯一的ProductKey。每个产品下可以有成千上万的设备。
        设备
         	归属于某个产品下的具体设备。物联网平台为设备颁发产品内唯一的证书DeviceName。设备可以直接连接物联网平台，也可以作为子设备通过网关连接物联网平台。
        设备证书
         	设备证书指ProductKey、DeviceName、DeviceSecret。
            ProductKey：是物联网平台为产品颁发的全局唯一标识。该参数很重要，在设备认证以及通信中都会用到，因此需要您保管好。
            DeviceName：在注册设备时，自定义的或自动生成的设备名称，具备产品维度内的唯一性。该参数很重要，在设备认证以及通信中都会用到，因此需要您保管好。
            DeviceSecret：物联网平台为设备颁发的设备密钥，和DeviceName成对出现。该参数很重要，在设备认证时会用到，因此需要您保管好并且不能泄露。
        Topic类
            同一产品下不同设备的Topic集合，用${productkey}和${deviceName}通配一个唯一的设备，一个Topic类对一个ProductKey下所有设备通用。
        
    2.mqtt平台的使用
设备链接:
    官方文档:
        https://help.aliyun.com/document_detail/96608.html?spm=a2c4g.11186623.6.605.69d8e2b0D7TxsW

    提供MQTT、CoAP、HTTP/S等多种协议的设备端SDK，既满足长连接的实时性需求，也满足短连接的低功耗需求
    开源多种平台设备端代码，提供跨平台移植指导，赋能企业基于多种平台做设备接入。
RRPC:
远程升级:
    升级过程支持进度条显示
    
    
