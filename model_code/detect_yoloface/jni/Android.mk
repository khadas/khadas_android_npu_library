########### build libnn_yoloface
LOCAL_PATH:= $(call my-dir)

include $(CLEAR_VARS)

common_src_files := vnn_yoloface.c yoloface_process.c yoloface.c 

common_c_includes := \
    $(LOCAL_PATH)/applib-ovxinc-include \
    $(LOCAL_PATH)/include \
    $(LOCAL_PATH)/service-ovx_inc \
    system/core/liblog/include \
    system/core/libutils/include 

#common_c_includes := \
	$(LOCAL_PATH)/include \
	system/core/libutils/include 



LOCAL_SRC_FILES := $(common_src_files)

LOCAL_C_INCLUDES := \
  $(common_c_includes) 

  
CFLAG+=-Wno-error=unused-variable
CFLAG+=-Wno-error=format
CFLAG+=-Wno-error=unused-function
CFLAG+=-Wno-error=unused-parameter
CFLAG+=-Wno-error=macro-redefined  
CFLAG+=-Wno-error=sign-compare
CFLAG+=-Wno-error=typedef-redefinition
LOCAL_CFLAGS += $(CFLAG)

LOCAL_LDLIBS += -llog -lz -lm -ldl
LOCAL_LDLIBS +=  -L$(LOCAL_PATH)/libs -lovxlib 
#LOCAL_LDLIBS +=  -lovxlib

LOCAL_MODULE := libnn_yoloface

#LOCAL_SHARED_LIBRARIES := \
    liblog \
    libcutils 

ifeq ($(shell test $(PLATFORM_SDK_VERSION) -ge 26 && echo OK),OK)
LOCAL_PROPRIETARY_MODULE := true
endif

LOCAL_PRELINK_MODULE := false
LOCAL_MODULE_TAGS := optional
LOCAL_VENDOR_MODULE := true


include $(BUILD_SHARED_LIBRARY)
