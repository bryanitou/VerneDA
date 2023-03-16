/**
 * NAMESPACE hosting problems formulas
 */
#pragma once

// DACE libraries
#include "dace/dace.h"

// Project libraries
#include "base/constants.h"
#include "tools/str.h"

namespace problems
{
    DACE::AlgebraicVector<DACE::DA> TwoBodyProblem(DACE::AlgebraicVector<DACE::DA> scv, double t );
    DACE::AlgebraicVector<DACE::DA> Attitude(DACE::AlgebraicVector<DACE::DA> scv, double t );

};
