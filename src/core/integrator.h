/**
 * Class having several types of integrators
 */

# pragma once

// Project libraries
#include "enums.h"

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
    template<typename T>
    DACE::AlgebraicVector<T> euler(DACE::AlgebraicVector<T> x, double t0, double t1);
};
