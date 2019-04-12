涉及源码:
    system/core/init/init.cpp
基础知识点:
    socketpair
    epoll

存在问题:
    1. init是如何被启动的
           在/proc/cmdline中指定
                init=/init
    1. 如何解析init.rc等一系列文件的

    2. 自定义的服务是如何启动的

    3. 自定义的服务未添加onshot关键字时，死掉能够重新启动是如何实现的

    4. 对on 属性:属性关键字 = 1,是如何实现

目前out目录下所有的rc文件
    ./root/init.rk3288.rc
    ./root/init.usb.configfs.rc
    ./root/init.rc
    ./root/init.rk30board.rc
    ./root/init.rk30board.environment.rc
    ./root/init.usb.rc
    ./root/init.rk30board.bootmode.nvme.rc
    ./root/init.rk30board.bootmode.emmc.rc
    ./root/init.environ.rc
    ./root/init.optee.rc
    ./root/init.rockchip.rc
    ./root/init.zygote32.rc
    ./root/init.connectivity.rc
    ./root/init.rk30board.usb.rc
    ./root/init.rk30board.bootmode.unknown.rc
    ./system/etc/init/init-debug.rc
    ./recovery/root/init.bootmode.unknown.rc
    ./recovery/root/init.rc
init树状图:
init.rc
    /init.environ.rc
    /init.usb.rc
    /init.rk30board.rc-->init.${ro.hardware}.rc
        init.rk30board.bootmode.emmc.rc-->init.${ro.hardware}.bootmode.${ro.bootmode}.rc
        init.rockchip.rc
        init.connectivity.rc
        init.box.samba.rc
        init.rk3288.rc-->init.${ro.board.platform}.rc
        init.tablet.rc-->init.${ro.target.product}.rc
        init.car.rc
        init.optee.rc
    /init.usb.configfs.rc
    init.zygote32.rc-->/init.${ro.zygote}.rc


