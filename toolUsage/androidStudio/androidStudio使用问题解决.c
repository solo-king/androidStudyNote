1.错误
    /dev/kvm permission denied. 
    解决办法:
    sudo apt-get install qemu-kvm 
    sudo adduser <当前用户> kvm
    sudo chown <当前用户> /dev/kvm 
android 8.1源码导入:
    参考资料:
        https://www.jianshu.com/p/9450806f38be
    涉及命令:
        source build/envsetup.sh
        lunch
        mmm development/tools/idegen/
        sudo sh ./development/tools/idegen/idegen.sh