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

DACE::AlgebraicVector<DACE::DA> problems::FreeTorqueMotion(DACE::AlgebraicVector<DACE::DA> scv, double t )
{
    // Create attitude and resultant vector
    DACE::AlgebraicVector<DACE::DA> q(4), omega(3), res(7);

    // Set positions: equal to the first three positions of the SCV (State Control Vector)
    q[0] = scv[0]; // Previous state
    q[1] = scv[1]; // Previous state
    q[2] = scv[2]; // Previous state
    q[3] = scv[3]; // Previous state

    // Normalize vector
    // TODO: Do this will work? It seems so..! Otherwise think of using algorithm in function quaternion::check_norm
    q = q.normalize();

    omega[0] = scv[4];
    omega[1] = scv[5];
    omega[2] = scv[6];

    // Set result
    res[0] = 0.5 * ( omega[2] * q[1] - omega[1] * q[2] + omega[0] * q[3]); // theta_x_dot
    res[1] = 0.5 * (-omega[2] * q[0] + omega[0] * q[2] + omega[1] * q[3]); // theta_x_dot
    res[2] = 0.5 * ( omega[1] * q[0] - omega[0] * q[1] + omega[2] * q[3]); // theta_x_dot
    res[3] = 0.5 * (-omega[0] * q[0] - omega[1] * q[1] - omega[2] * q[2]); // theta_x_dot
    res[4] = (Jy - Jz) * omega[1] * omega[2] / Jx; // omega_y_dot
    res[5] = (Jz - Jx) * omega[1] * omega[0] / Jy; // omega_z_dot
    res[6] = (Jx - Jy) * omega[0] * omega[1] / Jz; // omega_z_dot

    // Return result
    return res;
}

void problems::set_inertia_matrix(double inertia[3][3])
{
    // Show info to the user
    for (int i = 0; i < 3; i++)
    {
        for (int j = 0; j < 3; j++)
        {
            this->inertia_[i][j] = inertia[i][j];
            std::fprintf(stdout, "DEBUG: Inertia matrix I['%d']['%d'] = '%.5f'", i, j, this->inertia_[i][j]);
        }
    }
}

void problems::get_inverse_matrix(double a[3][3])
{
    // Get the determinant
    double det = problems::get_determinant(a);


}

double problems::get_determinant(double a[3][3])
{
    double det = a[0][0] * (a[1][1]*a[2][2] - a[1][2]*a[2][1])
                - a[0][1] * (a[1][0]*a[2][2] - a[2][0]*a[1][2])
                + a[0][2] * (a[1][0]*a[2][1] - a[1][1]*a[2][0]);

    // Return determinant
    return det;
}