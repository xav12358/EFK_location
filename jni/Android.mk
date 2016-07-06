LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

LOCAL_ARM_MODE := arm
LOCAL_LDLIBS := -L$(SYSROOT)/usr/lib -llog
LOCAL_MODULE    := EKF_Location
LOCAL_SRC_FILES := ekf_location.cpp ekf_location.h ekf_filter.cpp ekf_filter.h

include $(BUILD_SHARED_LIBRARY)
