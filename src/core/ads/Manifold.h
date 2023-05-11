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

// DACE libraries
#include "dace/dace.h"

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
    Manifold();

    explicit Manifold( unsigned int n );

    Manifold( const Manifold& m);

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
    void set_integrator_ptr(integrator* integrator);

public:
    // Getters
    integrator* get_integrator_ptr();

public:
    // Integrate until signal
    DACE::AlgebraicVector<DACE::DA> integrate_until_signal();

public: // Methods
    Manifold getSplitDomain(DACE::AlgebraicVector<DACE::DA> (*func)(DACE::AlgebraicVector<DACE::DA> ), const double errToll, const int nSplitMax, int posOverride = 0);

    Manifold getSplitDomain(DACE::AlgebraicVector<DACE::DA> (*func)(DACE::AlgebraicVector<DACE::DA>, double ), const double errtol, const int nSplitMax, const double mu, int posOverride = 0);

    Manifold getSplitDomain(DACE::AlgebraicVector<DACE::DA> (*func)(DACE::AlgebraicVector<DACE::DA>, double ), const std::vector<double> errToll, const int nSplitMax, const double mu, int posOverride = 0);

    Manifold getSplitDomain(DACE::AlgebraicVector<DACE::DA> (*func)(DACE::AlgebraicVector<DACE::DA>, Observable*, double), const double errToll, const int nSplitMax, Observable* param, const double mu, int posOverride = 0);

    Manifold getSplitDomain(DACE::AlgebraicVector<DACE::DA> (*func)(DACE::AlgebraicVector<DACE::DA>, Observable*, double), const std::vector<double> errToll, const int nSplitMax, Observable* param, const double mu, int posOverride = 0);
    
    Manifold* getSplitDomain(const std::vector<double>& errToll, int nSplitMax, int posOverride = 0);

    DACE::AlgebraicVector<double> pointEvaluationManifold(const DACE::AlgebraicVector<DACE::DA>& InitSet, DACE::AlgebraicVector<double> pt, const int flag = 0);

    void print_status();
};
