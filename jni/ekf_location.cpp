#include <ekf_filter.h>
#include <ekf_location.h>

#include <pthread.h>

static ekf_location EFKFilter;

JNIEXPORT void JNICALL sensorEvent(JNIEnv *env, jclass clazz, jfloat x, jfloat y, jfloat z, jint iTypeSensor,float f_dt)
{
	//LOGW("EKF_Location__OnLoad %.2f %.2f %.2f\n",x,y,z);
	if(iTypeSensor == 1)
	{
		EFKFilter.ekf_estimationGyrometer(x,y,z,f_dt);
		float theta,phi,psi;
		EFKFilter.getEuler(theta,phi,psi);
		LOGW("EKF_Location %.2f %.2f %.2f\n",theta,phi,psi);
	}
}

static JNINativeMethod gMethodRegistryMainActivity[] = {
		{ "sensorEvent", "(FFFIF)V", (void *) sensorEvent }
};


JNIEXPORT jint JNI_OnLoad(JavaVM* pVM, void* reserved) {
	JNIEnv *env;

	LOGW("EKF_Location__OnLoad\n");
	if ((pVM->GetEnv((void **)(&env), JNI_VERSION_1_6)) != JNI_OK)
	{
		//abort();
	}

	LOGW("JNI_OnLoad\n");

	jclass MainActivity = env->FindClass("com/example/ekf_location/MainActivity");
	if (MainActivity == NULL) abort();
	LOGW("JNI_OnLoad2\n");
	env->RegisterNatives( MainActivity,gMethodRegistryMainActivity, 1);
	LOGW("JNI_OnLoad3\n");
	env->DeleteLocalRef( MainActivity);
	LOGW("JNI_OnLoad4\n");

	return JNI_VERSION_1_6;
}
