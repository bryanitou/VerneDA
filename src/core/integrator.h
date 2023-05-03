/**
 * Class having several types of integrators
 */

# pragma once

// System libraries
#include <memory>

// Project libraries
#include "base/enums.h"
#include "problems.h"

// Project tools
#include "tools/vo.h"

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

private:
    std::vector<int> vector;

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
                                              double t0, double t1);

public:
    // Setters
    void set_problem_object(problems* probl);

private:
    // Private attributes

    // Class containing problems
    problems* probl_ = nullptr;

    // Number of variables to be used
    int nvar_{0};

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
    [[nodiscard]] DACE::AlgebraicVector<DACE::DA> euler(DACE::AlgebraicVector<DACE::DA> x,
                                          double t0, double t1) const;
    /**
     * This function will integrate using the RK4 (Range-Kutta 4) method
     * @param x             [in] [DACE::AlgebraicVector]
     * @param pFunction     [in] [pointer to function]
     * @param t0            [in] [double]
     * @param t1            [in] [double]
     * @return DACE::AlgebraicVector<DACE::DA>
     */
    DACE::AlgebraicVector<DACE::DA> RK4(DACE::AlgebraicVector<DACE::DA> x, double t0, double t1);

    template<typename T>
    DACE::AlgebraicVector<T> RK78(int N, DACE::AlgebraicVector<T> Y0, double X0, double X1);

    void print_detailed_information(const DACE::AlgebraicVector<DACE::DA> &x, int i, double t);

    DACE::AlgebraicVector<DACE::DA> RK4_step(const DACE::AlgebraicVector<DACE::DA>& x, double t, double h);

    DACE::AlgebraicVector<DACE::DA> Euler_step(const DACE::AlgebraicVector<DACE::DA> &x, double t, double h) const;
};
