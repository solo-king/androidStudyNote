1.错误
    /dev/kvm permission denied. 
    解决办法:
    sudo apt-get install qemu-kvm 
    sudo adduser <当前用户> kvm
    sudo chown <当前用户> /dev/kvm 