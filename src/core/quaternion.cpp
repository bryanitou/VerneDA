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
    this->q[0] = sin(roll/2) * cos(pitch/2) * cos(yaw/2) - cos(roll/2) * sin(pitch/2) * sin(yaw/2);
    this->q[1] = cos(roll/2) * sin(pitch/2) * cos(yaw/2) + sin(roll/2) * cos(pitch/2) * sin(yaw/2);
    this->q[2] = cos(roll/2) * cos(pitch/2) * sin(yaw/2) - sin(roll/2) * sin(pitch/2) * cos(yaw/2);
    this->q[3] = cos(roll/2) * cos(pitch/2) * cos(yaw/2) + sin(roll/2) * sin(pitch/2) * sin(yaw/2);
}

double *quaternion::inverse(double *q)
{
    // New double inversed
    auto* q2 = new double [4];

    double norm = q[0] * q[0]
                  + q[1] * q[1]
                  + q[2] * q[2]
                  + q[3] * q[3];

    q2 =  q[0] / norm;
    q2 = -q[1] / norm;
    q2 = -q[2] / norm;
    q2 = -q[3] / norm;

    return nullptr;
}

