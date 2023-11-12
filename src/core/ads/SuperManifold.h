/**
 * Super manifold class that will be dealing with manifold blocks.
 */

#pragma once

// Project libraries
#include "ads/Manifold.h"

class SuperManifold
{
public:
    // Class constructor
    /**
     * Default constructor.
     * @param current
     */
    explicit SuperManifold(ALGORITHM algorithm) : algorithm_(algorithm) {};
    explicit SuperManifold(const std::vector<double> &errToll, int nSplitMax, ALGORITHM algorithm)
    : errToll_(errToll), nSplitMax_(nSplitMax), algorithm_(algorithm) {};
    explicit SuperManifold(const double &nli_treshold, int nSplitMax, ALGORITHM algorithm)
            : nli_threshold_(nli_treshold), nSplitMax_(nSplitMax), algorithm_(algorithm) {};

    /**
     * Default destructor. TODO: Fix this destructor
     */
    ~ SuperManifold() = default;


public:// Attributes
    Manifold* previous_ = nullptr;
    Manifold* current_ = nullptr;
    Manifold* splitbox_ = nullptr;
    Manifold* att_6dof_fin = nullptr;
    Manifold* att_6dof_ini = nullptr;

private:
    // Tolerance error vector
    std::vector<double> errToll_; // ADS
    double nli_threshold_{}; // LOADS

    // Number of maximum splits, should be set to some integer
    int nSplitMax_{};

    // ADS/LOADS?
    ALGORITHM algorithm_{ALGORITHM::NA};

public:
    // Manifold operations
    void split_domain();

public:
    // Setters
    void set_integrator_ptr(integrator *integrator);

public:
    // Getters
    [[nodiscard]] Manifold* get_manifold_fin() const {return this->current_; };

    [[nodiscard]] Manifold* get_manifold_ini() const;

    [[nodiscard]] Manifold* get_att6dof_fin() const {return this->att_6dof_fin; };
    [[nodiscard]] Manifold* get_att6dof_ini() const {return this->att_6dof_ini; };

    void set_6dof_domain();

    void summary(std::string *summary2return, bool recursive);
};
