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
     * @return DACE::AlgebraicVector<DACE::DA>
     */
    DACE::AlgebraicVector<DACE::DA> integrate(const DACE::AlgebraicVector<DACE::DA>& x);

    /**
    * This function will use some of the available integrators (i.e., Euler, RK4)
    * @return DACE::AlgebraicVector<DACE::DA>
    */
    void integrate();

public:
    // Setters
    void set_problem_ptr(problems* problem);


    void set_integration_parameters(const DACE::AlgebraicVector<DACE::DA> &scv0, double t0, double t1,
                                    bool interrupt = false);

    void set_interrupt_flag(bool* flag);

    void set_errToll(const std::vector<double>& errToll);

    void set_nSplitMax(int nSplitMax)
    {
        this->nSplitMax_ = nSplitMax;
    }

public:
    // Getters
    problems* get_problem_ptr();

    DACE::AlgebraicVector<DACE::DA> get_scv()
    {
        return this->scv_;
    }

private:
    // Private attributes

    // Class containing the manifold
    problems* problem_ = nullptr;

    // Number of variables to be used
    int nvar_{0};

    // Integrator type
    INTEGRATOR type;

    // Step max
    double hmax_ = 0.1;

    // Demanar cita: 915412530

private:

    // Constant values

    // Initial scv
    DACE::AlgebraicVector<DACE::DA> scv_{};

    // Num. of steps_
    int steps_{};

    // Time increase: delta t
    double h_{};

    // Times: initial time and final time
    double t0_{};
    double t1_{};

    // Interrupt key
    bool interrupt_{false};

    // Parameters set?
    bool params_set_ = false;

    // Some control booleans
    std::vector<bool*> interrupt_flags_;

    // ADS constants
    std::vector<double> errToll_;
    int nSplitMax_;

private:
    /**
     * This function will integrate using the Euler method
     * @param x             [in] [DACE::AlgebraicVector]
     * @param pFunction     [in] [pointer to function]
     * @param t0            [in] [double]
     * @param t1            [in] [double]
     * @return DACE::AlgebraicVector<DACE::DA>
     */
    [[nodiscard]] DACE::AlgebraicVector<DACE::DA> Euler(DACE::AlgebraicVector<DACE::DA> x);
    /**
     * This function will integrate using the RK4 (Range-Kutta 4) method
     * @param x             [in] [DACE::AlgebraicVector]
     * @param pFunction     [in] [pointer to function]
     * @param t0            [in] [double]
     * @param t1            [in] [double]
     * @return DACE::AlgebraicVector<DACE::DA>
     */
    DACE::AlgebraicVector<DACE::DA> RK4(DACE::AlgebraicVector<DACE::DA> x);

    template<typename T>
    DACE::AlgebraicVector<T> RK78(int N, DACE::AlgebraicVector<T> Y0);

    void print_detailed_information(const DACE::AlgebraicVector<DACE::DA> &x, int i, double t);

    DACE::AlgebraicVector<DACE::DA> RK4_step(const DACE::AlgebraicVector<DACE::DA>& x, double t, double h);

    [[nodiscard]] DACE::AlgebraicVector<DACE::DA> Euler_step(const DACE::AlgebraicVector<DACE::DA> &x, double t, double h) const;

private:
    // Auxiliary functions
    bool check_interruption_flags();

    bool check_ads_conditions(const DACE::AlgebraicVector<DACE::DA> &x);
};
