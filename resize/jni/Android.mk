LOCAL_PATH:=$(call my-dir)
#
include $(CLEAR_VARS)

LOCAL_MODULE:= resize 
LOCAL_SRC_FILES:= resize.cpp
				   				   
include $(BUILD_EXECUTABLE)
