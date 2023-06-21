/********************************************************************************************/
//  Manifold.cpp                                                                             /
//  Manifold constructor and member function declaration                                      /
//                                                                                           /
//                                                                                           /
//  Created by Daniele Antonio Santeramo on 01/10/16.                                        /
//                                                                                           /
/********************************************************************************************/
#pragma once

// System libraries
#include <deque>
#include <algorithm>

// Project libraries
#include "Patch.h"
#include "integrator.h"

struct Observable;

/**
 * Manifold class gets inheritance from std::deque class.
 * @details https://en.cppreference.com/w/cpp/container/deque
 */
class Manifold : public std::deque<Patch>
{
public: // Constructors

    /**
     * Minimal constructor
     */
    Manifold();

    /**
     * Builds from another manifold.
     * @param m
     */
    Manifold(const Manifold& m);

    /**
     * Build class from Patch object
     * @param p [in] [patch]
     */
    explicit Manifold( const Patch& p);

    /**
     * Build class from DA Algebraic Vector
     * @param p [in] [DACE::AlgebraicVector<DACE::DA>]
     */
    explicit Manifold( const DACE::AlgebraicVector<DACE::DA>& p);

private:
    // Attributes
    integrator* integrator_ = nullptr;

public:
    // Setters

    /**
     * Sets integrator pointer
     * @param integrator [in] [integrator]
     */
    void set_integrator_ptr(integrator* integrator);

public:
    // Getters

    /**
     * Gets integrator pointer
     * \return integrator pointer
     */
    integrator* get_integrator_ptr() {return this->integrator_;};

public: // Methods

    /**
     * Main runninng class function, it splits the domains while integrating the dynamics of the object.
     * @param errToll       [in] [std::vector<double>]
     * @param nSplitMax     [in] [int]
     * @param posOverride   [in] [int]
     * @return Manifold*
     */
    Manifold* getSplitDomain(const std::vector<double>& errToll, int nSplitMax, int posOverride = 0);
    Manifold *getSplitDomain(ALGORITHM algorithm, int nSplitMax);

    /**
     * Evaluates a point in this manifold, returns the corresponding translation using the proper patch.
     * @param InitSet   [in] [DACE::AlgebraicVector<DACE::DA>]
     * @param pt        [in] [DACE::AlgebraicVector<DACE::DA>]
     * @param flag      [in] [int]
     * @return DACE::AlgebraicVector<double>
     */
    DACE::AlgebraicVector<double> pointEvaluationManifold(const DACE::AlgebraicVector<DACE::DA>& InitSet, DACE::AlgebraicVector<double> pt, int flag = 0);

    /**
     * Evaluates all the points in the center of the patches, returns them all transposed.
     * @return std::vector<DACE::AlgebraicVector<double>>
     */
    std::vector<DACE::AlgebraicVector<double>> centerPointEvaluationManifold();

    /**
     * Evaluates points at the domain edge of the patches (box), returns them all transposed.
     * @details Returns array per patch, per collection of wall points and the algebraic vector (point coordinates)
     * @return std::vector<std::vector<DACE::AlgebraicVector<double>>>
     */
    std::vector<std::vector<DACE::AlgebraicVector<double>>> wallsPointEvaluationManifold();

    /**
     * Prints status of the manifold, this routine is called from the main running routine 'getSplitDomain'
     */
    void print_status();

    Manifold *get_initial_split_domain();

    /*
     * GRAVEYARD
     */
    Manifold getSplitDomain(DACE::AlgebraicVector<DACE::DA> (*func)(DACE::AlgebraicVector<DACE::DA> ), const double errToll, const int nSplitMax, int posOverride = 0);

    Manifold getSplitDomain(DACE::AlgebraicVector<DACE::DA> (*func)(DACE::AlgebraicVector<DACE::DA>, double ), const double errtol, const int nSplitMax, const double mu, int posOverride = 0);

    Manifold getSplitDomain(DACE::AlgebraicVector<DACE::DA> (*func)(DACE::AlgebraicVector<DACE::DA>, double ), const std::vector<double> errToll, const int nSplitMax, const double mu, int posOverride = 0);

    Manifold getSplitDomain(DACE::AlgebraicVector<DACE::DA> (*func)(DACE::AlgebraicVector<DACE::DA>, Observable*, double), const double errToll, const int nSplitMax, Observable* param, const double mu, int posOverride = 0);

    Manifold getSplitDomain(DACE::AlgebraicVector<DACE::DA> (*func)(DACE::AlgebraicVector<DACE::DA>, Observable*, double), const std::vector<double> errToll, const int nSplitMax, Observable* param, const double mu, int posOverride = 0);

    std::vector<std::vector<DACE::AlgebraicVector<double>>> wallsPointEvaluationManifold_useless();

};
