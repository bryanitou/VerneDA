/**
 * Deltas will be computed in this namespace
 */
#pragma once

// Project libraries
#include "scv.h"

// System libraries
#include <random>

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
    void compute_deltas(DISTRIBUTION type, int n, POSITION pos = POSITION::X);

private:
    // Attributes
    std::shared_ptr<scv> scv_base_ = nullptr;

    // List of deltas
    std::shared_ptr<std::vector<std::shared_ptr<scv>>> scv_deltas = nullptr;

    // Polynomial to evaluate
    std::shared_ptr<DACE::AlgebraicVector<DACE::DA>> poly_ = nullptr;

private:
    // Allocators
    void allocate_scv_base(scv& scv_base, DACE::AlgebraicVector<DACE::DA>& poly);

private:
    // Deltas calculators relying on the distribution type
    void generate_gaussian_deltas(int n, POSITION pos);

    // Evaluate
    void evaluate_deltas(POSITION pos);
};
