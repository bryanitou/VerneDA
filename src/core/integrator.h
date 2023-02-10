/**
 * Class having several types of integrators
 */

# pragma once

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
    template<typename T>
    DACE::AlgebraicVector<DACE::DA> euler(DACE::AlgebraicVector<DACE::DA> x, std:: double t0, double t1);
};
