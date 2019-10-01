
@echo off
adb reboot bootloader

fastboot flash vbmeta_a vbmeta.img
fastboot flash vbmeta_b vbmeta.img
fastboot flash dtbo_a dtbo.img
fastboot flash dtbo_b dtbo.img
fastboot flash boot_a boot.img
fastboot flash boot_b boot.img
fastboot flash system_a system.img -S 512M
fastboot flash system_b system.img -S 512M
fastboot flash vendor_a vendor.img -S 512M
fastboot flash vendor_b vendor.img -S 512M
fastboot format userdata 
fastboot reboot
fastboot flashing unlock
fastboot reboot
adb root
adb disable-verity
adb reboot  

