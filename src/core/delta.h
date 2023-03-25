/**
 * Deltas will be computed in this namespace
 */
#pragma once

// System libraries
#include <random>
#include <cmath>
#include <unordered_map>

// Project libraries
#include "scv.h"
#include "tools/ep.h"
#include "quaternion.h"
#include "tools/str.h"

class delta {

public:

    /**
     * Class constructor
     * @param scv
     */
    explicit delta(scv& scv_base, DACE::AlgebraicVector<DACE::DA>& poly);

    /**
     * Default destructor
     */
    ~delta() = default;

public:

    /**
     * Set constants for the deltas generation
     * @param mean_pos [in] [double]
     * @param stddev_pos [in] [double]
     * @param mean_vel [in] [double]
     * @param stddev_vel [in] [double]
     */
    void set_constants(double mean_pos, double stddev_pos, double mean_vel, double stddev_vel);

    template<typename T> void set_option(DELTA_GENERATOR_OPTION option,  T value)
    {
        // If case relying on the option
        if (option == DELTA_GENERATOR_OPTION::ATTITUDE)
        {
            this->attitude_ = value;
        }
        else if (option == DELTA_GENERATOR_OPTION::QUAT2EULER)
        {
            this->quat2euler_ = value;
        }
    }


    void set_option_sampling(QUATERNION_SAMPLING q_sampling_option) {this->q_sampling_ = q_sampling_option;}

    /**
     * Insert the nominal SCV StateControlVector
     * @param scv_nominal [in] [scv]
     */
    void insert_nominal(const scv& scv_nominal);

    /**
     * Compute the deltas. Constants need to be set for this.
     * @param type [in] [DISTRIBUTION]
     * @param n [in] [int]
     * @param state [in] [STATE]
     */
    void generate_deltas(DISTRIBUTION type, int n);

    // Evaluate
    void evaluate_deltas();

public: // Getters
    /**
     * Get evaluated deltas polynomial.
     * @return deltas_poly
     */
    std::shared_ptr<std::vector<DACE::AlgebraicVector<DACE::DA>>> get_eval_deltas_poly()
    {
        return eval_deltas_poly_;
    };

    /**
     * Get not evaluated deltas polynomial.
     * @return not evaluated deltas scv
     */
    std::shared_ptr<std::vector<std::shared_ptr<scv>>> get_non_eval_deltas_poly()
    {
        return scv_deltas_;
    };

private:
    // Attributes
    // Initial SCV (StateControlVector)
    std::shared_ptr<scv> scv_base_ = nullptr;
    // Polynomial to evaluate: TRANSFORMATION TAYLOR POLYNOMIAL
    std::shared_ptr<DACE::AlgebraicVector<DACE::DA>> base_poly_ = nullptr;

    // List of deltas: not evaluated
    std::shared_ptr<std::vector<std::shared_ptr<scv>>> scv_deltas_ = nullptr;
    // List of results:
    std::shared_ptr<std::vector<DACE::AlgebraicVector<DACE::DA>>> eval_deltas_poly_ = nullptr;

public:
    std::vector<double> mean_state_;

private:

    // Distribution constants
    double mean_pos_{};
    double stddev_pos_{};
    double mean_vel_{};
    double stddev_vel_{};

    // Constants set?
    bool constants_set_{false};

    // Nominal inserted?
    bool nominal_inserted_{false};

    // Options for this class
    bool attitude_{false};
    bool quat2euler_{false};
    QUATERNION_SAMPLING q_sampling_{QUATERNION_SAMPLING::NA};

private:
    // Allocators
    void allocate_scv_base(scv& scv_base, DACE::AlgebraicVector<DACE::DA>& poly);

private:
    // Deltas calculators relying on the distribution type
    void generate_gaussian_deltas(int n);

};
