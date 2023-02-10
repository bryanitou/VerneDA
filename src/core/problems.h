/**
 * NAMESPACE hosting problems formulas
 */
#pragma once

// DACE libraries
#include "dace/dace.h"

namespace problems
{
    DACE::AlgebraicVector<DACE::DA> TwoBodyProblem( DACE::AlgebraicVector<DACE::DA> x, double t );

};
