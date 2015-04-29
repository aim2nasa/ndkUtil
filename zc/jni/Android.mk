LOCAL_PATH:=$(call my-dir)
#
include $(CLEAR_VARS)

LOCAL_MODULE:= zc 
LOCAL_SRC_FILES:= zc.cpp

LOCAL_LDLIBS+=-lz
LOCAL_C_INCLUDES+=/cygdrive/c/android-ndk-r10d/platforms/android-21/arch-arm/usr/include/
				   				   
include $(BUILD_EXECUTABLE)
