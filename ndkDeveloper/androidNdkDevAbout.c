参考资料:
    //官方资料
    https://developer.android.com/ndk/guides/prebuilts

    NDK documentation, guides, and API reference are available on
    https://developer.android.com/ndk/index.html

    NDK code samples are available on
    https://github.com/googlesamples/android-ndk

--sysroot=dir
           Use dir as the logical root directory for headers and libraries.  For example, if the compiler normally searches for headers in /usr/include and libraries in /usr/lib, it instead searches
           dir/usr/include and dir/usr/lib.

Android 8.1 (API level 27)

独立编译工具链#########################################################################
    $NDK/toolchains/llvm/prebuilt/linux-x86_64/bin/aarch64-linux-android27-clang++ src.cpp
编译出可执行文件:
    OK
编译出第三方动态库:
    
使用so库编译出可执行文件:
使用so库编译出动态库:




