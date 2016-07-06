#ifndef __EKF_FILTER_H__
#define __EKF_FILTER_H__




#include <Eigen/Dense>
using Eigen::MatrixXd;
using Eigen::VectorXd;
using Eigen::Quaterniond;
class ekf_location
{

	bool IsReady;
	Quaterniond q_SE;
	VectorXd b_MagDirection;

	double ax,ay,az;
	double mx,my,mz;
//	MatrixXd Q_processnoise;
//	MatrixXd P_covariance;
//	float f_varianceAccelerometer;
//	float f_varianceGyrometer;
//	float f_varianceMagnetometer;

	float f_beta;
	float f_zeta;

	// Alignment matrix for magnetometers
	MatrixXd M_Mag;
	VectorXd B_Mag;

public:
	ekf_location();
	~ekf_location();

	void ekf_initialisationIMU();
	void ekf_estimationAccelerometer(double ax,double ay,double az);
	void ekf_estimationGyrometer(double wx,double wy,double wz,float f_dt);
	void ekf_estimationMagnetometer(double mx,double my,double mz);
	void ekf_prediction();
	void setAccelerometer(double ax0,double ay0,double az0){
		ax = ax0;
		ay = ay0;
		az = az0;
	};

	void setMagnetometer(double mx0,double my0,double mz0){
		mx = mx0;
		my = my0;
		mz = mz0;
	};

	void getEuler(float &theta,float &phi, float &psi);
};


#endif
