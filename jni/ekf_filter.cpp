#include <ekf_filter.h>
#include <ekf_location.h>


ekf_location::ekf_location():
q_SE(1,0,0,0),
f_beta(0.001),
f_zeta(0.001),
b_MagDirection(3),
M_Mag(3,3),
B_Mag(3),
IsReady(false)
{
	M_Mag = MatrixXd::Identity(3,3);
}


ekf_location::~ekf_location()
{

}



void ekf_location::ekf_initialisationIMU()
{
	static int icount = 0;


	if(icount >1000)
	{
		IsReady =  true;
	}

}



void ekf_location::ekf_estimationAccelerometer(double ax,double ay,double az)
{
	VectorXd Fg(3,1);
	Fg(0) = 2*(q_SE.w()*q_SE.z() - q_SE.w()*q_SE.y()) - ax;
	Fg(1) = 2*(q_SE.w()*q_SE.x() + q_SE.y()*q_SE.z()) - ay;
	Fg(2) = 2*(0.5-q_SE.x()*q_SE.x() - q_SE.y()*q_SE.y()) - az;

	MatrixXd Jf(3,4);
	Jf(0,0) = -2*q_SE.y();
	Jf(0,1) = 2*q_SE.z();
	Jf(0,2) = -2*q_SE.w();
	Jf(0,3) = 2*q_SE.x();

	Jf(1,0) = 2*q_SE.x();
	Jf(1,1) = 2*q_SE.w();
	Jf(1,2) = 2*q_SE.z();
	Jf(1,3) = 2*q_SE.y();

	Jf(2,0) = 0;
	Jf(2,1) = -4*q_SE.x();
	Jf(2,2) = -4*q_SE.y();
	Jf(2,3) = 0;

	VectorXd q_SE_update = f_beta*Jf.transpose()*Fg;

	q_SE.w() = q_SE.w()+ q_SE_update(0);
	q_SE.x() = q_SE.x()+ q_SE_update(1);
	q_SE.y() = q_SE.y()+ q_SE_update(2);
	q_SE.z() = q_SE.z()+ q_SE_update(3);
	q_SE.normalize();
}


void ekf_location::ekf_estimationGyrometer(double wx,double wy,double wz,float f_dt)
{
	f_dt = fmin(f_dt,0.007);
	Quaterniond q_Sw(-wz*0.5,-wx*0.5,-wy*0.5,0);
	LOGW("----------\nEKF_Location1 %.5f %.5f %.5f %.5f\n",q_SE.x(),q_SE.y(),q_SE.z(),q_SE.w());
	//LOGW("EKF_Location2 %.5f %.5f %.5f %.5f\n",q_Sw.x(),q_Sw.y(),q_Sw.z(),q_Sw.w());
//	Quaterniond q_SE_update = q_SE*q_Sw;
//	q_SE.x() = q_SE.x() + q_SE_update.x()*f_dt;
//	q_SE.y() = q_SE.y() + q_SE_update.y()*f_dt;
//	q_SE.z() = q_SE.z() + q_SE_update.z()*f_dt;
//	q_SE.x() = q_SE.w() + q_SE_update.w()*f_dt;

	//wx = -wx;
	//wy = -wy;
	//wz = 0;//-wz;
	Quaterniond q_ES = q_SE;
	//q_ES.inverse();
	q_SE.x() = q_ES.x() + (0.5*(-q_ES.y()*wx -q_ES.z()*wy-q_ES.w()*wz))*f_dt;
	q_SE.y() = q_ES.y() + (0.5*(q_ES.x()*wx +q_ES.z()*wz-q_ES.w()*wy ))*f_dt;
	q_SE.z() = q_ES.z() + (0.5*(q_ES.x()*wy -q_ES.y()*wz+q_ES.w()*wx ))*f_dt;
	q_SE.w() = q_ES.w() + (0.5*(q_ES.x()*wz +q_ES.y()*wy-q_ES.z()*wx ))*f_dt;


	//LOGW("EKF_Location3 %.2f %.2f %.2f %.2f\n",q_SE.x(),q_SE.y(),q_SE.z(),q_SE.w());
	q_SE.normalize();
	//LOGW("EKF_Location4 %.2f %.2f %.2f %.2f\n",q_SE.x(),q_SE.y(),q_SE.z(),q_SE.w());
}


void ekf_location::ekf_estimationMagnetometer(double mx,double my,double mz)
{
	VectorXd Fg(3,1);
	Fg(0) = 2*b_MagDirection(0)*(0.5-q_SE.y()*q_SE.y()-q_SE.z()*q_SE.z())+ 2*b_MagDirection(2)*(q_SE.x()*q_SE.z()-q_SE.w()*q_SE.y()) -mx;
	Fg(1) = 2*b_MagDirection(0)*(q_SE.x()*q_SE.y()-q_SE.w()*q_SE.z()) + 2*b_MagDirection(2)*(q_SE.w()*q_SE.x()+q_SE.y()*q_SE.z()) -my;
	Fg(2) = 2*b_MagDirection(0)*(q_SE.w()*q_SE.y()-q_SE.x()*q_SE.z())+ 2*b_MagDirection(2)*(0.5-q_SE.x()*q_SE.x()-q_SE.y()*q_SE.y()) -mz;

	MatrixXd Jf(3,4);
	Jf(0,0) = -2*b_MagDirection(2)*q_SE.y();
	Jf(0,1) = 2*b_MagDirection(2)*q_SE.z();
	Jf(0,2) = -4*b_MagDirection(0)*q_SE.y()-2*b_MagDirection(2)*q_SE.w();
	Jf(0,3) = -4*b_MagDirection(0)*q_SE.z()+2*b_MagDirection(2)*q_SE.x();

	Jf(1,0) = -2*b_MagDirection(0)*q_SE.z()+2*b_MagDirection(2)*q_SE.x();
	Jf(1,1) = 2*b_MagDirection(0)*q_SE.y()+2*b_MagDirection(2)*q_SE.w();
	Jf(1,2) = 2*b_MagDirection(0)*q_SE.x()+2*b_MagDirection(2)*q_SE.z();
	Jf(1,3) = -2*b_MagDirection(0)*q_SE.w()+2*b_MagDirection(2)*q_SE.y();

	Jf(2,0) = 2*b_MagDirection(0)*q_SE.y();
	Jf(2,1) = 2*b_MagDirection(0)*q_SE.z()-4*b_MagDirection(2)*q_SE.x();
	Jf(2,2) = 2*b_MagDirection(0)*q_SE.w()-4*b_MagDirection(2)*q_SE.y();
	Jf(2,3) = 2*b_MagDirection(0)*q_SE.x();

	VectorXd q_SE_update = f_zeta*Jf.transpose()*Fg;

	q_SE.w() = q_SE.w()+ q_SE_update(0);
	q_SE.x() = q_SE.x()+ q_SE_update(1);
	q_SE.y() = q_SE.y()+ q_SE_update(2);
	q_SE.z() = q_SE.z()+ q_SE_update(3);
	q_SE.normalize();
}


void ekf_location::ekf_prediction()
{

}


void ekf_location::getEuler(float &theta,float &phi, float &psi)
{
    phi   = atan2(2*(q_SE.y()*q_SE.z()-q_SE.x()*q_SE.w()),(2*(q_SE.x()*q_SE.x()+q_SE.y()*q_SE.y())-1));//*(180/3.14159);
    theta = -asin(2*(q_SE.y()*q_SE.w()+q_SE.x()*q_SE.z()));//*(180/3.14159);
    psi   = atan2(2*(q_SE.z()*q_SE.w()-q_SE.x()*q_SE.y()),2*(q_SE.x()*q_SE.x()+q_SE.w()*q_SE.w())-1);//*(180/3.14159);

	/*Quaterniond q_ES = q_SE;
	//q_ES.conjugate();
    float R1_1 = 2*q_ES.x()*q_ES.x()-1+2*q_ES.y()*q_ES.y();
    float R2_1 = 2*(q_ES.y()*q_ES.z()-q_ES.x()*q_ES.w());
    float R3_1 = 2*(q_ES.y()*q_ES.w()+q_ES.x()*q_ES.z());
    float R3_2 = 2*(q_ES.z()*q_ES.w()-q_ES.x()*q_ES.y());
    float R3_3 = 2*q_ES.x()*q_ES.x()-1+2*q_ES.w()*q_ES.w();


    phi 	= atan2(R3_2, R3_3 )*(180/3.14159);
    theta 	= -atan(R3_1 / sqrt(1-R3_1*R3_1) )*(180/3.14159);
    psi 	= atan2(R2_1, R1_1 )*(180/3.14159);
*/
    float thetao = theta * cos(phi) + psi * sin(phi);
    float psio = -theta * sin(phi) + psi * cos(phi);

    theta =thetao*(180/3.14159);
    psi = psio*(180/3.14159);
    phi = phi*(180/3.14159);
}
