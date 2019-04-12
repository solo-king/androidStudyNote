LOCAL_PATH:= $(call my-dir)

include $(CLEAR_VARS)

LOCAL_SRC_FILES:= \
	BnNiaoRenService.cpp \
	BpNiaoRenService.cpp \
	clientTest.cpp

LOCAL_SHARED_LIBRARIES := \
	libcutils \
	libutils \
	liblog \
	libbinder 

LOCAL_MODULE:= clientTest
LOCAL_32_BIT_ONLY := true
include $(BUILD_EXECUTABLE)

include $(CLEAR_VARS)

LOCAL_SRC_FILES:= \
	BnNiaoRenService.cpp \
	BpNiaoRenService.cpp \
	serviceTest.cpp

LOCAL_SHARED_LIBRARIES := \
	libcutils \
	libutils \
	liblog \
	libbinder 

LOCAL_MODULE:= serviceTest
LOCAL_32_BIT_ONLY := true
include $(BUILD_EXECUTABLE)
