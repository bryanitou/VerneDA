/**
 * Class having several types of integrators
 */

# pragma once

// System libraries
#include <memory>

// Project libraries
#include "base/enums.h"

// DACE libraries
#include "dace/dace.h"

class integrator {

public:
    /**
     * Class constructor.
     * @param integrator
     */
    explicit integrator(INTEGRATOR integrator, double stepmax);

    /**
     * Default destructor
     */
    ~integrator() = default;

public:
    /**
     * This function will use some of the available integrators (i.e., Euler, RK4)
     * @param x             [in] [DACE::AlgebraicVector]
     * @param pFunction     [in] [pointer to function]
     * @param t0            [in] [double]
     * @param t1            [in] [double]
     * @return DACE::AlgebraicVector<DACE::DA>
     */
    DACE::AlgebraicVector<DACE::DA> integrate(const DACE::AlgebraicVector<DACE::DA>& x,
                                              DACE::AlgebraicVector<DACE::DA>
                                                      (*pFunction)(DACE::AlgebraicVector<DACE::DA>, double),
                                              double t0, double t1);

private:
    // Private attributes

    // Integrator type
    INTEGRATOR type;

    // Step max
    double hmax = 0.1;

private:
    /**
     * This function will integrate using the Euler method
     * @param x             [in] [DACE::AlgebraicVector]
     * @param pFunction     [in] [pointer to function]
     * @param t0            [in] [double]
     * @param t1            [in] [double]
     * @return DACE::AlgebraicVector<DACE::DA>
     */
    DACE::AlgebraicVector<DACE::DA> euler(DACE::AlgebraicVector<DACE::DA> x,
                                          DACE::AlgebraicVector<DACE::DA>
                                                  (*formula)(DACE::AlgebraicVector<DACE::DA>, double),
                                          double t0, double t1) const;
    /**
     * This function will integrate using the RK4 (Range-Kutta 4) method
     * @param x             [in] [DACE::AlgebraicVector]
     * @param pFunction     [in] [pointer to function]
     * @param t0            [in] [double]
     * @param t1            [in] [double]
     * @return DACE::AlgebraicVector<DACE::DA>
     */
    DACE::AlgebraicVector<DACE::DA> RK4(DACE::AlgebraicVector<DACE::DA> x,
                                        DACE::AlgebraicVector<DACE::DA>
                                                (*pFunction)(DACE::AlgebraicVector<DACE::DA>,
                                                                                     double), double t0, double t1) const;

    template<typename T>
    DACE::AlgebraicVector<T> RK78(int N, DACE::AlgebraicVector<T> Y0,
                                  DACE::AlgebraicVector<DACE::DA> (*pFunction)(DACE::AlgebraicVector<DACE::DA>, double),
                                  double X0, double X1);
};
