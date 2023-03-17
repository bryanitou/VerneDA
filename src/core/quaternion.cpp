/**
 * Quaternions class.
 */

#include "quaternion.h"

std::vector<double> quaternion::euler2quaternion(double roll, double pitch, double yaw)
{
    // Pre-compute values for efficiency
    double sin_roll = sin(roll/2);
    double cos_roll = cos(roll/2);
    double sin_pitch = sin(pitch/2);
    double cos_pitch = cos(pitch/2);
    double sin_yaw = sin(yaw/2);
    double cos_yaw = cos(yaw/2);

    // Declare quaternion
    std::vector<double> q1(4);

    // Save the quaternion
    q1[1] = sin_roll * cos_pitch * cos_yaw - cos_roll * sin_pitch * sin_yaw;
    q1[2] = cos_roll * sin_pitch * cos_yaw + sin_roll * cos_pitch * sin_yaw;
    q1[3] = cos_roll * cos_pitch * sin_yaw - sin_roll * sin_pitch * cos_yaw;
    q1[0] = cos_roll * cos_pitch * cos_yaw + sin_roll * sin_pitch * sin_yaw;

    // Return got quaternion
    return q1;
}

void quaternion::scale(DACE::AlgebraicVector<DACE::DA>* q, double num)
{
    // Save the quaternion
    q[0].normalize() = q[0] * num;
    q[1].normalize() = q[1] * num;
    q[2].normalize() = q[2] * num;
    q[3].normalize() = q[3] * num;

}

void quaternion::check_norm(DACE::AlgebraicVector<DACE::DA>* q)
{
    // Check the norm of the quaternion
    auto q_norm = q->cons().vnorm();
    auto line2write = tools::string::print2string("Integration point N^2(q) = '%.5f'", q_norm);

    // TODO: Debug line
    std::cout << line2write << std::endl;

    // Take square
    double qmagsq = q_norm*q_norm;

    // Check error
    double err = std::abs(1.0 - qmagsq);

    // Evaluate
    if (err < 2.107342e-08)
    {
        // Scale
        quaternion::scale(q, 2.0 / (1.0 + qmagsq));
    }
    else
    {
        // Scale
        quaternion::scale(q, 1.0 / std::sqrt(qmagsq));
    }
}

double quaternion::getnorm(std::vector<double> q)
{
    double norm =   q[0] * q[0]
                    + q[1] * q[1]
                    + q[2] * q[2]
                    + q[3] * q[3];

    return norm;
}

/*
std::vector<double> quaternion::quaternion2euler(double a, double b, double c, double d)
{
    // Declare quaternion
    std::vector<double> euler_angles(3);

    double yaw = atan2 (2.0*( c * d + a * b ) , a * a - b * b - c * c + d * d);

    double t0 = +2.0 * (w * x + y * z);
    double t1 = +1.0 - 2.0 * (x * x + y * y);
    double roll_x = atan2(t0, t1);

    double t2 = +2.0 * (w * y - z * x);
    // Python code translated below:
    // double t2 = +1.0 if t2 > +1.0 else t2;
    // double t2 = -1.0 if t2 < -1.0 else t2;
    // Becomes:
    t2 = t2 > +1.0 ? +1.0 : t2 < -1.0 ? -1.0 : t2;
    double pitch_y = asin(t2);
    double t3 = +2.0 * (w * z + x * y);
    double t4 = +1.0 - 2.0 * (y * y + z * z);
    double yaw_z = math.atan2(t3, t4);


    // Return got quaternion
    return q1;
}*/


std::vector<double>  quaternion::inverse(std::vector<double> q2inv)
{
    // New double inversed
    std::vector<double> q_inv(4);

    double norm = quaternion::getnorm(q2inv);

    q_inv[0] = q2inv[0] / norm;
    q_inv[1] = -q2inv[1] / norm;
    q_inv[2] = -q2inv[2] / norm;
    q_inv[3] = -q2inv[3] / norm;

    return q_inv;
}

std::vector<double> quaternion::q8_multiply (std::vector<double>  q1, std::vector<double>  q2)
{
    std::vector<double> q3(4);

    q3[0] = q1[0] * q2[0] - q1[1] * q2[1] - q1[2] * q2[2] - q1[3] * q2[3];
    q3[1] = q1[0] * q2[1] + q1[1] * q2[0] + q1[2] * q2[3] - q1[3] * q2[2];
    q3[2] = q1[0] * q2[2] - q1[1] * q2[3] + q1[2] * q2[0] + q1[3] * q2[1];
    q3[3] = q1[0] * q2[3] + q1[1] * q2[2] - q1[2] * q2[1] + q1[3] * q2[0];

    return q3;
}