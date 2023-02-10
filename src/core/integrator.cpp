/**
 * Class having several types of integrators
 */

#include "integrator.h"

integrator::integrator(INTEGRATOR integrator)
{
    // Set type
    this->type = integrator;
}


DACE::AlgebraicVector<DACE::DA> integrator::euler(DACE::AlgebraicVector<DACE::DA> x,
                                                  DACE::AlgebraicVector<DACE::DA> (*formula)(DACE::AlgebraicVector<DACE::DA>, double),
                                                  double t0, double t1)
{
    // Step max
    const double hmax = 0.1;

    // Num of steps
    int steps = ceil((t1-t0)/hmax);

    // Height? TODO: Investigate what was this
    double h = (t1-t0)/steps;

    // Current time
    double t = t0;

    // Iterate
    for( int i = 0; i < steps; i++ )
    {
        x = x + h * (formula(x, t));
        t += h;
    }

    return x;
}