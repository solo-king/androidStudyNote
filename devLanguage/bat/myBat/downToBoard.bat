@echo off
set vendorPath="/vendor/firmware"
set fwPath="os.checked_1.bin"

::print infomation of env
echo env:
echo [vendorPath]%vendorPath% 
echo [fwPath]%fwPath% 

adb remount vendor

if not %errorlevel%==0 (
    echo "remount error!!!"
    pause
) 

adb push %fwPath% %vendorPath%
 
if not %errorlevel%==0 (
    echo "push error!!!"
    pause
) 

set /p selected=reboot HW?[y/n]
if %selected%==y (
    adb reboot
)

echo %0 succeed!!!
pause


