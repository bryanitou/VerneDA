/**
 * NAMESPACE hosting problems formulas
 * - Quaternions problem: https://ieeexplore.ieee.org/stamp/stamp.jsp?tp=&arnumber=686806
 */

#include "problems.h"

DACE::AlgebraicVector<DACE::DA> problems::TwoBodyProblem(DACE::AlgebraicVector<DACE::DA> scv, double t )
{
    // Create position and resultant vector
    DACE::AlgebraicVector<DACE::DA> pos(3), res(6);

    // Set positions: equal to the first three positions of the SCV (State Control Vector)
    pos[0] = scv[0]; // Px_dot
    pos[1] = scv[1]; // Py_dot
    pos[2] = scv[2]; // Pz_dot
    auto r = pos.vnorm();

    res[0] = scv[3]; // Px_dot = Vx
    res[1] = scv[4]; // Py_dot = Vy
    res[2] = scv[5]; // Pz_dot = Vz

    // Compute next Vx, Vy, Vz state from the current position
    res[3] = -constants::earth::mu*pos[0]/(r*r*r); // Vx_dot
    res[4] = -constants::earth::mu*pos[1]/(r*r*r); // Vy_dot
    res[5] = -constants::earth::mu*pos[2]/(r*r*r); // Vz_dot

    // Return result
    return res;
}

DACE::AlgebraicVector<DACE::DA> problems::Attitude(DACE::AlgebraicVector<DACE::DA> scv, double t )
{
    // Create attitude and resultant vector
    DACE::AlgebraicVector<DACE::DA> q(4), omega(3), res(7);

    // Set positions: equal to the first three positions of the SCV (State Control Vector)
    q[0] = scv[0]; // ?
    q[1] = scv[1]; // ?
    q[2] = scv[2]; // ?
    q[3] = scv[3]; // ?

    double r = 2;
    double h = 6;
    double mass = 1000;
    double Jx = (mass * h * h) / 12 + (mass * r * r) / 4;
    double Jy = (mass * h * h) / 12 + (mass * r * r) / 4;
    double Jz = (mass * r * r) / 4;

    // Set result
    // Don't touch this
    res[0] = 0.5 * ( omega[2] * q[1] - omega[1] * q[2] + omega[0] * q[3]); // theta_x_dot
    res[1] = 0.5 * (-omega[2] * q[0] + omega[0] * q[2] + omega[1] * q[3]); // theta_x_dot
    res[2] = 0.5 * ( omega[1] * q[0] - omega[0] * q[1] + omega[2] * q[3]); // theta_x_dot
    res[3] = 0.5 * (-omega[0] * q[0] - omega[1] * q[1] - omega[2] * q[2]); // theta_x_dot
    res[4] = (Jy - Jz) * omega[1] * omega[2] / Jx; // omega_y_dot
    res[5] = (Jz - Jx) * omega[1] * omega[0] / Jy; // omega_z_dot
    res[6] = (Jx - Jy) * omega[0] * omega[1] / Jz; // omega_z_dot

    // Check norm of the new quaternion
    // Check the norm of the quaternion
    auto q_res_norm = q.cons().extract(0, 3).vnorm();
    auto line2write = tools::string::print2string("Integration point: \n\t'%.5f'", q_res_norm);

    std::cout << line2write << std::endl;

    omega[0] = scv[4];
    omega[1] = scv[5];
    omega[2] = scv[6];

    // Return result
    return res;
}