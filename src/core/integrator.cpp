/**
 * Class having several types of integrators
 */

#include "integrator.h"

integrator::integrator(INTEGRATOR integrator, double stepmax)
{
    // Set type
    this->type = integrator;

    // Set step max
    this->hmax = stepmax;
}


DACE::AlgebraicVector<DACE::DA> integrator::euler(DACE::AlgebraicVector<DACE::DA> x,
                                                  DACE::AlgebraicVector<DACE::DA> (*formula)(DACE::AlgebraicVector<DACE::DA>, double),
                                                  double t0, double t1) const
{
    // Num of steps
    int steps = std::ceil((t1-t0)/this->hmax);

    // Delta time? TODO: Investigate what was this
    double h = (t1 - t0) / steps;

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

DACE::AlgebraicVector<DACE::DA> integrator::RK4(DACE::AlgebraicVector<DACE::DA> x,
                                                  DACE::AlgebraicVector<DACE::DA> (*pFunction)(DACE::AlgebraicVector<DACE::DA>, double),
                                                  double t0, double t1) const
{
    // Auxiliary variables for the loop
    DACE::AlgebraicVector<DACE::DA> k1;
    DACE::AlgebraicVector<DACE::DA> k2;
    DACE::AlgebraicVector<DACE::DA> k3;
    DACE::AlgebraicVector<DACE::DA> k4;

    // Num of steps
    int steps = std::ceil((t1-t0)/this->hmax);

    // Delta time? TODO: Investigate what was this
    double h = (t1 - t0) / steps;

    // Current time
    double t = t0;

    // Iterate
    for( int i = 0; i < steps; i++ )
    {
        // Compute points in between
        k1 = pFunction(x, t);
        k2 = pFunction(x + h * (k1/3), t + h/3);
        k3 = pFunction(x + h * (-k1/3 + k2), t + 2*h/3);
        k4 = pFunction(x + h * (k1 - k2 + k3), t + h);

        // Compute the single step
        x = x + h * (k1 + 3*k2 + 3*k3 + k4)/8;

        // Increase step time
        t += h;
    }

    return x;
}

DACE::AlgebraicVector<DACE::DA> integrator::integrate(const DACE::AlgebraicVector<DACE::DA>& x,
                                                      DACE::AlgebraicVector<DACE::DA> (*pFunction)(
                                                              DACE::AlgebraicVector<DACE::DA>, double), double t0,
                                                      double t1) {
    DACE::AlgebraicVector<DACE::DA> result;

    switch (this->type)
    {
        case INTEGRATOR::EULER:
        {
            result = this->euler(x, pFunction, t0, t1);
            break;
        }
        case INTEGRATOR::RK4:
        {
            result = this->RK4(x, pFunction, t0, t1);
            break;
        }

    }

    return result;
}

