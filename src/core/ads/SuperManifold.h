/**
 * Super manifold class that will be dealing with manifold blocks.
 */

#pragma once

// System libraries

// Project libraries
#include "Manifold.h"

class SuperManifold
{
public:
    // Class constructor
    /**
     * Default constructor.
     * @param current
     */
    explicit SuperManifold(const std::vector<double> &errToll, int nSplitMax) : errToll_(errToll), nSplitMax_(nSplitMax) {};

    /**
     * Default destructor. TODO: Fix this destructor
     */
    ~ SuperManifold() = default;

private:
    // Constant values

    // Tolerance error vector
    std::vector<double> errToll_;

    // Number of maximum splits, should be set to some integer
    int nSplitMax_;

private:
    // Attributes
    Manifold* previous_ = nullptr;
    Manifold* current_ = nullptr;

public:
    // Manifold operations
    void split_domain();

public:

    /**
     * Set initial manifold.
     * @param manifold
     */
    void set_initial_manifold(Manifold* manifold);

    void set_integrator_ptr(integrator *integrator);

public:
    // Getters
    // std::vector<double> get_results();
};
