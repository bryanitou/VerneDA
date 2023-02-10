/**
 * Class having several types of integrators
 */

#include "integrator.h"

integrator::integrator(INTEGRATOR integrator)
{
    // Set type
    this->type = integrator;
}


template<typename T>
DACE::AlgebraicVector<DACE::DA> integrator::euler(DACE::AlgebraicVector<DACE::DA> x, double t0, double t1)
{
    const double hmax = 0.1;
    int steps = ceil((t1-t0)/hmax);
    double h = (t1-t0)/steps;
    double t = t0;
    for( int i = 0; i < steps; i++ )
    {
        x = x + h * TBP(x,t);
        t += h;
    }
    return x;
}