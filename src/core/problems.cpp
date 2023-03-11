/**
 * NAMESPACE hosting problems formulas
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

    res[0] = scv[3]; // Vx_dot
    res[1] = scv[4]; // Vy_dot
    res[2] = scv[5]; // Vz_dot

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
    DACE::AlgebraicVector<DACE::DA> ang(3), res(6);

    // Set positions: equal to the first three positions of the SCV (State Control Vector)
    ang[0] = scv[0]; // theta_x_dot
    ang[1] = scv[1]; // theta_y_dot
    ang[2] = scv[2]; // theta_z_dot

    // Set result
    res[0] = ang[0]; // theta_x_dot
    res[1] = ang[1]; // theta_y_dot
    res[2] = ang[2]; // theta_z_dot
    res[3] = 0.0; // omega_x_dot
    res[4] = 0.0; // omega_y_dot
    res[5] = 0.0; // omega_z_dot

    // Return result
    return res;
}