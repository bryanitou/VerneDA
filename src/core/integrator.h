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
    explicit integrator(INTEGRATOR integrator);

    /**
     * Default destructor
     */
    ~integrator() = default;

private:
    INTEGRATOR type;

public:
    static DACE::AlgebraicVector<DACE::DA> euler(DACE::AlgebraicVector<DACE::DA> x,
                                          DACE::AlgebraicVector<DACE::DA> (*formula)(DACE::AlgebraicVector<DACE::DA>, double),
                                          double t0, double t1);
};
