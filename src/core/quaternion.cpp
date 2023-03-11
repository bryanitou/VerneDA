/**
 * Quaternions class.
 */

#include "quaternion.h"

quaternion::quaternion(double roll, double pitch, double yaw)
{
    // Create the quaternion
    auto q1 = get_from_Euler(roll, pitch, yaw);

    // Save it
    this->set_quaternion(q1);
}

void quaternion::set_quaternion(double *q_new)
{
    // Set received quaternion
    this->q[0] = q_new[0];
    this->q[1] = q_new[1];
    this->q[2] = q_new[2];
    this->q[3] = q_new[3];

    // Delete quaternion
    delete q_new;
}

double * quaternion::get_from_Euler(double roll, double pitch, double yaw)
{
    // Pre-compute values for efficiency
    double sin_roll = sin(roll/2);
    double cos_roll = cos(roll/2);
    double sin_pitch = sin(pitch/2);
    double cos_pitch = cos(pitch/2);
    double sin_yaw = sin(yaw/2);
    double cos_yaw = cos(yaw/2);

    // Declare quaternion
    auto * q1 = new double[4];

    // Save the quaternion
    q1[0] = sin_roll * cos_pitch * cos_yaw - cos_roll * sin_pitch * sin_yaw;
    q1[1] = cos_roll * sin_pitch * cos_yaw + sin_roll * cos_pitch * sin_yaw;
    q1[2] = cos_roll * cos_pitch * sin_yaw - sin_roll * sin_pitch * cos_yaw;
    q1[3] = cos_roll * cos_pitch * cos_yaw + sin_roll * sin_pitch * sin_yaw;

    // Return got quaternion
    return q1;
}

double *quaternion::inverse()
{
    // Return result
    return quaternion::inverse(this->q);
}

double *quaternion::inverse(const double *q2inv)
{
    // New double inversed
    auto* q_inv = new double [4];

    double norm =   q2inv[0] * q2inv[0]
                  + q2inv[1] * q2inv[1]
                  + q2inv[2] * q2inv[2]
                  + q2inv[3] * q2inv[3];

    q_inv[0] = q2inv[0] / norm;
    q_inv[1] = -q2inv[1] / norm;
    q_inv[2] = -q2inv[2] / norm;
    q_inv[3] = -q2inv[3] / norm;

    return q_inv;
}

double *q8_multiply ( const double q1[], const double q2[] )
{
    double *q3;

    q3 = new double[4];

    q3[0] = q1[0] * q2[0] - q1[1] * q2[1] - q1[2] * q2[2] - q1[3] * q2[3];
    q3[1] = q1[0] * q2[1] + q1[1] * q2[0] + q1[2] * q2[3] - q1[3] * q2[2];
    q3[2] = q1[0] * q2[2] - q1[1] * q2[3] + q1[2] * q2[0] + q1[3] * q2[1];
    q3[3] = q1[0] * q2[3] + q1[1] * q2[2] - q1[2] * q2[1] + q1[3] * q2[0];

    return q3;
}

double* quaternion::rotate(double roll, double pitch, double yaw)
{
    // Get the rotation axis as a quaternion
    auto q_axis = quaternion::get_from_Euler(roll, pitch, yaw);

    // Get the inverse
    return quaternion::rotate(q_axis);
}

double* quaternion::rotate(const double *q_axis)
{
    // Get the inverse
    auto q_inv = quaternion::inverse(q_axis);

    // Multiply
    auto res1 = q8_multiply(this->q, q_inv);
    auto q_res = q8_multiply(q_axis, res1);

    // Return result
    return q_res;
}