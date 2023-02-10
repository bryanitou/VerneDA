/**
 * NAMESPACE hosting problems formulas
 */
#pragma once

// DACE libraries
#include "dace/dace.h"

namespace problems
{
    template<typename T> DACE::AlgebraicVector<DACE::DA>
            TwoBodyProblem( DACE::AlgebraicVector<T> x, double t );

};
