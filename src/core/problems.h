/**
 * NAMESPACE hosting problems formulas
 */
#pragma once

// System libraries
#include <cstdlib>

// DACE libraries
#include "dace/dace.h"

// Project libraries
#include "base/constants.h"
#include "base/enums.h"
#include "tools/str.h"
#include "tools/ep.h"
#include "tools/vo.h"
#include "quaternion.h"
#include "json/json_parser.h"

class problems
{
public: // Constructor

    /**
     * Main constructor, saves problem type and mu constant
     * @param type
     * @param mu
     */
    explicit problems(PROBLEM type, double mu = constants::earth::mu);

    // Destructor
    ~problems();

public:
    // Solve problems
    DACE::AlgebraicVector<DACE::DA> solve(const DACE::AlgebraicVector<DACE::DA>& scv, double t);

private:
    // Problems
    DACE::AlgebraicVector<DACE::DA> TwoBodyProblem(DACE::AlgebraicVector<DACE::DA> scv, double t) const;
    static DACE::AlgebraicVector<DACE::DA> FreeFallObject(DACE::AlgebraicVector<DACE::DA> scv, double t);
    DACE::AlgebraicVector<DACE::DA> FreeTorqueMotion(DACE::AlgebraicVector<DACE::DA> scv, double t);

    // Static transformations
    /**
     * Polar to cartesian coordinates in 2D
     * @param pol [in] [DACE::AlgebraicVector<DACE::DA>]
     * @return  DACE::AlgebraicVector<DACE::DA>
    */
    static DACE::AlgebraicVector<DACE::DA> pol2cart(DACE::AlgebraicVector<DACE::DA> pol);

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

    // Mu to be set...
    double mu_{};

    static double** get_inverse_matrix(double **a);

    static double get_determinant(double **a);

    static void memory_frees(double **a);

    DACE::AlgebraicVector<DACE::DA> get_cross_product(DACE::AlgebraicVector<DACE::DA> omega);

public:
    void summary(std::string *summary2return, bool recursive);
};
