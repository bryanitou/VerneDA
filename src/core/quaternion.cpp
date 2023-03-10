/**
 * Quaternions class.
 */

#include "quaternion.h"

quaternion::quaternion(double roll, double pitch, double yaw)
{
    this->set_from_Euler(roll, pitch, yaw);
}

void quaternion::set_from_Euler(double roll, double pitch, double yaw)
{
    // Pre-compute values for efficiency
    double sin_roll = sin(roll/2);
    double cos_roll = cos(roll/2);
    double sin_pitch = sin(pitch/2);
    double cos_pitch = cos(pitch/2);
    double sin_yaw = sin(yaw/2);
    double cos_yaw = cos(yaw/2);

    // Save the quaternion
    this->q[0] = sin_roll * cos_pitch * cos_yaw - cos_roll * sin_pitch * sin_yaw;
    this->q[1] = cos_roll * sin_pitch * cos_yaw + sin_roll * cos_pitch * sin_yaw;
    this->q[2] = cos_roll * cos_pitch * sin_yaw - sin_roll * sin_pitch * cos_yaw;
    this->q[3] = cos_roll * cos_pitch * cos_yaw + sin_roll * sin_pitch * sin_yaw;
}

double *quaternion::inverse()
{
    // New double inversed
    auto* q2 = new double [4];

    double norm =     this->q[0] * this->q[0]
                      + this->q[1] * this->q[1]
                      + this->q[2] * this->q[2]
                      + this->q[3] * this->q[3];

    q2[0] =  this->q[0] / norm;
    q2[1] = -this->q[1] / norm;
    q2[2] = -this->q[2] / norm;
    q2[3] = -this->q[3] / norm;

    // Return result
    return q2;
}

double *quaternion::inverse(const double *q)
{
    // New double inversed
    auto* q2 = new double [4];

    double norm =   q[0] * q[0]
                  + q[1] * q[1]
                  + q[2] * q[2]
                  + q[3] * q[3];

    q2[0] =  q[0] / norm;
    q2[1] = -q[1] / norm;
    q2[2] = -q[2] / norm;
    q2[3] = -q[3] / norm;

    return q2;
}

