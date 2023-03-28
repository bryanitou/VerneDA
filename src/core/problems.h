/**
 * NAMESPACE hosting problems formulas
 */
#pragma once

// DACE libraries
#include "dace/dace.h"

// Project libraries
#include "base/constants.h"
#include "tools/str.h"
#include "quaternion.h"

class problems
{
public:
    // Constructor
    problems() = default;

    // Destructor
    ~problems() = default;

public:
    // Problems
    DACE::AlgebraicVector<DACE::DA> TwoBodyProblem(DACE::AlgebraicVector<DACE::DA> scv, double t );
    DACE::AlgebraicVector<DACE::DA> FreeTorqueMotion(DACE::AlgebraicVector<DACE::DA> scv, double t );

public:
    // Setters
    void set_inertia_matrix(double inertia[3][3]);

private:
    // Attributes
    double inertia_[3][3];
};
