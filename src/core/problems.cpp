/**
 * NAMESPACE hosting problems formulas
 */

#include "problems.h"

DACE::AlgebraicVector<DACE::DA> problems::TwoBodyProblem(DACE::AlgebraicVector<DACE::DA> x, double t )
{
    DACE::AlgebraicVector<DACE::DA> pos(3), res(6);
    pos[0] = x[0]; pos[1] = x[1]; pos[2] = x[2];
    auto r = pos.vnorm();
    const double mu = 398600; // km^3/s^2
    res[0] = x[3]; res[1] = x[4]; res[2] = x[5];
    res[3] = -mu*pos[0]/(r*r*r);
    res[4] = -mu*pos[1]/(r*r*r);
    res[5] = -mu*pos[2]/(r*r*r);
    return res;
}