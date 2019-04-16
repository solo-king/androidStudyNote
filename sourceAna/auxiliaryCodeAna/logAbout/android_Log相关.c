c++层log打印:
    使用步骤:
        1.在需要打印的源码第一行定义log标志
            #define LOG_TAG "StrongPointMain"
        2.包含头文件
            #include <cutils/log.h>
        3.在Android.mk中添加库支持
            LOCAL_SHARED_LIBRARIES := \
                libcutils 
    使用:
        1.几种等级调试输出
            ALOGI("[age]=%d",age);
            ALOGD("[age]=%d",age);
            ALOGW("[age]=%d",age);
            ALOGE("[age]=%d",age);
        2.自动添加换行，不需要再换行
