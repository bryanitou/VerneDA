/**
 * NAMESPACE hosting problems formulas
 */

#include "problems.h"

DACE::AlgebraicVector<DACE::DA> problems::TwoBodyProblem(DACE::AlgebraicVector<DACE::DA> scv, double t )
{
    // Create position and resultant vector
    DACE::AlgebraicVector<DACE::DA> pos(3), res(6);

    // Set positions: equal to the first three positions of the SCV (State Control Vector)
    pos[0] = scv[0]; pos[1] = scv[1]; pos[2] = scv[2];
    auto r = pos.vnorm();

    res[0] = scv[3]; res[1] = scv[4]; res[2] = scv[5];
    res[3] = -constants::earth::mu*pos[0]/(r*r*r);
    res[4] = -constants::earth::mu*pos[1]/(r*r*r);
    res[5] = -constants::earth::mu*pos[2]/(r*r*r);
    return res;
}