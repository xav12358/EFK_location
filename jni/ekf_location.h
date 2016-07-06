#ifndef __EKF_H__
#define __EKF_H__

#include <jni.h>
#include <android/bitmap.h>
#include <android/log.h>

#define app_name "ekf_location"
#define LOGI(...) ((void)__android_log_print(ANDROID_LOG_INFO, app_name, __VA_ARGS__))
#define LOGW(...) ((void)__android_log_print(ANDROID_LOG_WARN, app_name, __VA_ARGS__))
#define LOGE(...) ((void)__android_log_print(ANDROID_LOG_ERROR, app_name, __VA_ARGS__))

JNIEXPORT void JNICALL sensorEvent(JNIEnv *env, jclass clazz, jfloat x, jfloat y, jfloat z, jint iTypeSensor,float f_dt);
JNIEXPORT void JNICALL gpsEvent(JNIEnv *env, jclass clazz, jdouble latitude, jdouble longitude, jdouble altitude, float f_dt);


#endif
