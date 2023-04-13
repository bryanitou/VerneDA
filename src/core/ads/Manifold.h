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
#include "dace/AlgebraicVector.h"
#include "dace/DA.h"

// Project libraries
#include "Patch.h"

struct Observable;

class Manifold : public std::deque< Patch > {
public:
    Manifold();

    Manifold( unsigned int n );

    Manifold( const Manifold& m);

    Manifold( const Patch& p);

    Manifold( const DACE::AlgebraicVector<DACE::DA>& p);

    Manifold getSplitDomain(DACE::AlgebraicVector<DACE::DA> (*func)(DACE::AlgebraicVector<DACE::DA> ), const double errToll, const int nSplitMax, int posOverride = 0);

    Manifold getSplitDomain(DACE::AlgebraicVector<DACE::DA> (*func)(DACE::AlgebraicVector<DACE::DA>, double ), const double errtol, const int nSplitMax, const double mu, int posOverride = 0);

    Manifold getSplitDomain(DACE::AlgebraicVector<DACE::DA> (*func)(DACE::AlgebraicVector<DACE::DA>, double ), const std::vector<double> errToll, const int nSplitMax, const double mu, int posOverride = 0);

    Manifold getSplitDomain(DACE::AlgebraicVector<DACE::DA> (*func)(DACE::AlgebraicVector<DACE::DA>, Observable*, double), const double errToll, const int nSplitMax, Observable* param, const double mu, int posOverride = 0);

    Manifold getSplitDomain(DACE::AlgebraicVector<DACE::DA> (*func)(DACE::AlgebraicVector<DACE::DA>, Observable*, double), const std::vector<double> errToll, const int nSplitMax, Observable* param, const double mu, int posOverride = 0);
    
    Manifold getSplitDomain(DACE::AlgebraicVector<DACE::DA> (*func)(DACE::AlgebraicVector<DACE::DA>, double, double), const std::vector<double> errToll, const int nSplitMax, const double Dt, const double mu, int posOverride = 0);

    DACE::AlgebraicVector<double> pointEvaluationManifold( DACE::AlgebraicVector<DACE::DA> InitSet, DACE::AlgebraicVector<double> pt, const int flag = 0);
};
