/**
 * Class having several types of integrators
 */


# pragma once

// System libraries
#include <memory>
#include <fstream>

// Project libraries
#include "base/enums.h"
#include "problems.h"

// Project tools
#include "tools/vo.h"
#include "tools/ep.h"

// DACE libraries
#include "dace/dace.h"
#include "dace/AlgebraicMatrix_t.h"

class integrator {

public:
    /**
     * Class constructor.
     * @param integrator
     */
    explicit integrator(INTEGRATOR integrator, ALGORITHM algorithm,  double stepmax);

    /**
     * Default destructor
     */
    ~integrator() = default;

public:
    double t_{};
    bool end_{false};
    double nli_current_;
    std::vector<int> vector;

public:
    /**
     * This function will use some of the available integrators (i.e., Euler, RK4)
     * @param x             [in] [DACE::AlgebraicVector]
     * @return DACE::AlgebraicVector<DACE::DA>
     */
    DACE::AlgebraicVector<DACE::DA> integrate(const DACE::AlgebraicVector<DACE::DA>& x, int patch_id = - 1);

public:
    // Setters
    void set_problem_ptr(problems* problem);


    void set_integration_parameters(const DACE::AlgebraicVector<DACE::DA> &scv0, double t0, double t1,
                                    bool interrupt = false);

    void set_errToll(const std::vector<double>& errToll);

    void set_nli_threshold(const double &nli_threshold);

    void set_beta(std::vector<double> &beta)
    {
        this->betas_ = beta;

        auto betas_str = tools::vector::num2string(this->betas_);
        std::fprintf(stdout, "Set betas in Integrator: %s\n", betas_str.c_str());
    }

public:
    // Getters
    problems* get_problem_ptr();

    DACE::AlgebraicVector<DACE::DA> get_scv()
    {
        return this->scv_;
    }

    [[nodiscard]] int get_splitting_pos() const
    {
        return this->pos_;
    }

    auto get_algorithm() {return this->algorithm_;}

public: // SAFETY CHECK FUNCTIONS
    void summary(std::string * summary2return, bool recursive);

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

public:
    // Betas vector
    std::vector<double> betas_{};

private:

    // Constant values

    // Initial scv
    DACE::AlgebraicVector<DACE::DA> scv_{};

    // Num. of steps_
    int steps_{};

    // Time increase: delta t
    double h_{};

    // Times: initial time and final time
    double t0_{};   // Initial time
    double t1_{};   // Final time
    // Current time

    // Interrupt key
    bool interrupt_{false};

    // Parameters set?
    bool params_set_ = false;

    // ADS/LOADS common stuff
    int pos_;

    // ADS constants
    std::vector<double> errToll_;

    // ADS/LOADS algorithm
    ALGORITHM algorithm_{ALGORITHM::NA};

    // LOADS stuff
    double nli_threshold_;

private:
    // Some auxilary class variables
    int patch_id_ = -1;

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

    /**
    * Check conditons: ADS or LOADS, switch case.
    * @param x
    * @param debug
    * @return
    */
    bool check_conditions(const DACE::AlgebraicVector<DACE::DA> &x, bool debug = false);

    /**
     * Check ADS conditions
     * @param x
     * @return
     */
    bool check_ads_conditions(const DACE::AlgebraicVector<DACE::DA> &x);

    /**
     * Check LOADS conditions
     * @param x
     * @param debug
     * @return
     */
    bool check_loads_conditions(const DACE::AlgebraicVector<DACE::DA> &x, bool debug = false);

    /**
     * Static transformation
     * @param x
     * @return
     */
    DACE::AlgebraicVector<DACE::DA> static_transformation(DACE::AlgebraicVector<DACE::DA> x);
};
