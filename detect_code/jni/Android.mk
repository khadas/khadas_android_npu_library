########### build libkhadas_npu_jni
LOCAL_PATH:= $(call my-dir)

include $(CLEAR_VARS)
  

common_src_files := khadas_npu_det.cpp detect.c detect_log.c 


common_c_includes := \
	$(LOCAL_PATH)/include \
	system/core/liblog/include \
	libnativehelper/include_jni \
	system/core/libutils/include 

	
LOCAL_SRC_FILES := $(common_src_files)

LOCAL_C_INCLUDES := $(common_c_includes) 

  
CFLAG+=-Wno-error=unused-variable
CFLAG+=-Wno-error=format
CFLAG+=-Wno-error=unused-function
CFLAG+=-Wno-error=unused-parameter
CFLAG+=-Wno-error=macro-redefined  
CFLAG+=-Wno-error=sign-compare 
CFLAG+=-Wno-error=missing-braces 
CFLAG+=-Wno-error=uninitialized 
CFLAG+=-Wno-error=implicit-int 
CFLAG+=-Wno-error=implicit-function-declaration
CFLAG+=-Wno-error=int-conversion

LOCAL_CFLAGS += $(CFLAG)
LOCAL_LDLIBS += -llog -lz -lm -ldl
LOCAL_LDLIBS += -L$(LOCAL_PATH)/opencv_libs -lopencv_java4  

LOCAL_MODULE := libkhadas_npu_jni

		
ifeq ($(shell test $(PLATFORM_SDK_VERSION) -ge 26 && echo OK),OK)
LOCAL_PROPRIETARY_MODULE := true
endif

LOCAL_PRELINK_MODULE := false
LOCAL_MODULE_TAGS := optional
LOCAL_VENDOR_MODULE := true


include $(BUILD_SHARED_LIBRARY)
