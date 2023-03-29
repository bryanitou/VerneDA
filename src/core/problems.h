/**
 * NAMESPACE hosting problems formulas
 */
#pragma once

// DACE libraries
#include "dace/dace.h"

// Project libraries
#include "base/constants.h"
#include "base/enums.h"
#include "tools/str.h"
#include "quaternion.h"

class problems
{
public:
    // Constructor
    explicit problems(PROBLEM type);

    // Destructor
    ~problems();

public:
    DACE::AlgebraicVector<DACE::DA> solve(const DACE::AlgebraicVector<DACE::DA>& scv, double t);

private:
    // Problems
    static DACE::AlgebraicVector<DACE::DA> TwoBodyProblem(DACE::AlgebraicVector<DACE::DA> scv, double t);
    DACE::AlgebraicVector<DACE::DA> FreeTorqueMotion(DACE::AlgebraicVector<DACE::DA> scv, double t);

public:
    // Setters
    void set_inertia_matrix(double inertia[3][3]);

public:
    // Getters
    PROBLEM get_type(){return this->type_;}

private:
    // Attributes
    double** inertia_;
    double** inverse_;

    // Type of problem
    PROBLEM type_{PROBLEM::NA};

    static double** get_inverse_matrix(double **a);

    static double get_determinant(double **a);

    static void memory_frees(double **a);

    DACE::AlgebraicVector<DACE::DA> get_cross_product(DACE::AlgebraicVector<DACE::DA> omega);
};
