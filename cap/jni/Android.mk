LOCAL_PATH:=$(call my-dir)
#
include $(CLEAR_VARS)

LOCAL_MODULE:= cap 
LOCAL_SRC_FILES:= cap.cpp
LOCAL_LDLIBS += -lz
LOCAL_SHARED_LIBRARIES := libjpeg
LOCAL_C_INCLUDES+= /cygdrive/c/android-ndk-r10d/platforms/android-21/arch-arm/usr/include/
				   				   
include $(BUILD_EXECUTABLE)
