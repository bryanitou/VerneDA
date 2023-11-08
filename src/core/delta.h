/**
 * Deltas will be computed in this namespace
 */
#pragma once

// System libraries
#include <random>
#include <cmath>
#include <unordered_map>
#include <utility>

// Project libraries
#include "scv.h"
#include "tools/ep.h"
#include "ads/SuperManifold.h"

class delta {

public:

    /**
     * Class constructor
     * @param scv
     */
    explicit delta() = default;

    /**
     * Default destructor
     */
    ~delta() = default;

public: // Set constants

    /**
     * Set standard deviations for the deltas generation
     * @param stddevs [in] [std::vector<double>]
     */
    void set_stddevs(const std::vector<double>& stddevs);
    // void set_stddevs_q(const std::vector<double>& stddevs);

public: // Set options
    /**
     * Set bool options necessary for this class
     * @param option [in] [DELTA_GENERATOR_OPTION]
     * @param value [in] [bool]
     */
    void set_bool_option(DELTA_GENERATOR_OPTION option, bool value);

    /**
     * Set sampling option.
     * @param q_sampling_option
     */
    void set_sampling_option(QUATERNION_SAMPLING q_sampling_option) { this->q_sampling_ = q_sampling_option; }

    /**
     * Set mean quaternion option, necessary for the attitude.
     * @param mean_q
     */
    void set_mean_quaternion_option(std::vector<double> mean_q);

    /**
     * Insert the nominal SCV StateControlVector
     * @param n [in] [scv]
     */
    void insert_nominal(int n);
    void insert_nominal(const DACE::AlgebraicVector<double>& n);

    /**
     * Compute the deltas. Constants need to be set for this.
     * @param type [in] [DISTRIBUTION]
     * @param n [in] [int]
     * @param state [in] [STATE]
     */
    void generate_deltas(DISTRIBUTION type, int n);

    void set_superManifold(SuperManifold* sm)
    {
        this->sm_ = sm;
    };

    /**
     * Evaluate prepared deltas
     */
    void evaluate_deltas();

public: // Getters
    /**
     * Get evaluated deltas polynomial.
     * @return deltas_poly
     */
    auto get_eval_deltas_poly()
    {
        return eval_deltas_poly_;
    };

    /**
     * Get not evaluated deltas polynomial.
     * @return not evaluated deltas scv
     */
    auto get_non_eval_deltas_poly(bool scale = false)
    {
        // De-activated code
        if (scale && false)
        {
            for (auto & s : *this->scv_deltas_)
            {
                s = s * 13356.27;
            }
        }

        // Return initial deltas
        return this->scv_deltas_;
    };

    /**
     * Return saved manifold in this class
     * @return SuperManifold
     */
    SuperManifold* get_SuperManifold()
    {
        return this->sm_;
    };

public:

    // General methods
    void convert_non_eval_deltas_to_euler();

private:
    // List of deltas: not evaluated
    std::shared_ptr<std::vector<DACE::AlgebraicVector<double>>> scv_deltas_ = nullptr;
    // List of results:
    std::shared_ptr<std::vector<DACE::AlgebraicVector<double>>> eval_deltas_poly_ = nullptr;

private:

    // Distribution constants
    std::vector<double> stddevs_{};

    // Constants set?
    bool stddevs_set_{false};

    // Nominal inserted?
    bool zeroed_inserted_{false};

    // Options for this class
    bool attitude_{false};
    bool quat2euler_{false};
    QUATERNION_SAMPLING q_sampling_{QUATERNION_SAMPLING::NA};
    std::vector<double> mean_quaternion_{};
    std::vector<double> mean_euler_{};

private:
    // Other important objects
    SuperManifold* sm_ = nullptr;

private: // Allocators
    /**
     * Allocate scv base
     * @param scv_base
     * @param poly
     */
    void allocate_scv_base(scv& scv_base, DACE::AlgebraicVector<DACE::DA>& poly);

private: // Class functions
    /**
     * Deltas calculators relying on the distribution type
     * @param n
     */
    void generate_gaussian_deltas(int n);

private: // Safety checks

    /**
     * Safety checks for attitude engine
     */
    void attitude_safety_checks();
};
