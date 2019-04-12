参考资料:
    官方参考资料:
        http://android.mk/
    将APP集成到固件中参考资料:
        https://blog.csdn.net/u010218230/article/details/79153986
    使用三方库或jar:
        https://blog.csdn.net/thl789/article/details/7918093
疑问:
    1.静态jar包
    2.
变量描述:
    LOCAL_PATH := $(call my-dir)
        is used to return the path of the current directory (i.e. the directory containing the Android.mk file itself).
    include $(CLEAR_VARS)
        will clear many LOCAL_XXX variables for you (e.g. LOCAL_MODULE, LOCAL_SRC_FILES, LOCAL_STATIC_LIBRARIES, etc...), 
        with the exception of LOCAL_PATH. This is needed because all build control files are parsed in a single GNU Make execution context where all variables are global.
    LOCAL_MODULE := hello-jni
APK编译:
    http://android.mk/#simpleAPK
    LOCAL_PRIVILEGED_MODULE := true
        决定apk安装位置,false:/system/app
                                    true:/system/priv-app
    包含AIDL文件:
        LOCAL_SRC_FILES := $(call all-java-files-under, src) $(call all-Iaidl-files-under, src)
    LOCAL_MANIFEST_FILE := app/src/main/AndroidManifest.xml
编译应用参考模板
    LOCAL_PATH := $(call my-dir)
    include $(CLEAR_VARS)
    
    res_dirs = app/src/main/res
    
    LOCAL_MANIFEST_FILE := app/src/main/AndroidManifest.xml
    LOCAL_SRC_FILES := $(call all-java-files-under,src) $(call all-Iaidl-files-under, src)\
                            app/src/main/aidl/cn/usr/www/usrbroadcastcentor/BookManager.aidl
    LOCAL_RESOURCE_DIR := $(addprefix $(LOCAL_PATH)/, $(res_dirs))
    
    LOCAL_PACKAGE_NAME := UsrBroadcastCentor
    LOCAL_MODULE_TAGS := eng
    LOCAL_PROGUARD_ENABLED := disabled
    LOCAL_CERTIFICATE := platform
    LOCAL_PRIVILEGED_MODULE := true
    LOCAL_DEX_PREOPT := false
    include $(BUILD_PACKAGE)