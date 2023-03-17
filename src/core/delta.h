/**
 * Deltas will be computed in this namespace
 */
#pragma once

// Project libraries
#include "scv.h"
#include "tools/ep.h"
#include "quaternion.h"
#include "tools/str.h"

// System libraries
#include <random>
#include <cmath>

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


    /**
     * Compute the deltas. Constants need to be set for this.
     * @param type [in] [DISTRIBUTION]
     * @param n [in] [int]
     * @param state [in] [STATE]
     */
    void compute_deltas(DISTRIBUTION type, int n, STATE state,  bool attitude = false, bool quat2euler = false);

    /**
     * Get deltas polynomial.
     * @return deltas_poly
     */
    std::shared_ptr<std::vector<DACE::AlgebraicVector<DACE::DA>>> get_deltas_poly()
    {
        return deltas_poly_;
    };

private:
    // Attributes
    std::shared_ptr<scv> scv_base_ = nullptr;
    // Polynomial to evaluate
    std::shared_ptr<DACE::AlgebraicVector<DACE::DA>> base_poly_ = nullptr;

    // List of deltas
    std::shared_ptr<std::vector<std::shared_ptr<scv>>> scv_deltas_ = nullptr;
    // List of results
    std::shared_ptr<std::vector<DACE::AlgebraicVector<DACE::DA>>> deltas_poly_ = nullptr;

private:

    // Distribution constants
    double mean_pos_{};
    double stddev_pos_{};
    double mean_vel_{};
    double stddev_vel_{};

    // Constants set?
    bool constants_set{false};

private:
    // Allocators
    void allocate_scv_base(scv& scv_base, DACE::AlgebraicVector<DACE::DA>& poly);

private:
    // Deltas calculators relying on the distribution type
    void generate_gaussian_deltas(int n, STATE state,  bool attitude);

    // Evaluate
    void evaluate_deltas(bool quat2euler);
};
