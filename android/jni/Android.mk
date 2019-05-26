LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)
LOCAL_MODULE    := libOpenAL
LOCAL_SRC_FILES := ../external/libopenal.a 
include $(PREBUILT_STATIC_LIBRARY)

include $(CLEAR_VARS)
LOCAL_MODULE    		:= neurojam
LOCAL_SRC_FILES 		:= ../../source/main.c libopenal.a
LOCAL_LDLIBS    		:= -llog -landroid -lEGL -lGLESv2 -lOpenSLES
LOCAL_STATIC_LIBRARIES 	:= android_native_app_glue libOpenAL
LOCAL_C_INCLUDES 		:= ../source

LOCAL_CFLAGS += -std=c99
LOCAL_CFLAGS += -O2
LOCAL_CFLAGS += -DNDEBUG

include $(BUILD_SHARED_LIBRARY)

$(call import-module,android/native_app_glue)
