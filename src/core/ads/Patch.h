/********************************************************************************************/
//  Patch.cpp                                                                             /
//  Patch constructor and member function declaration                                      /
//                                                                                           /
//                                                                                           /
//  Created by Daniele Antonio Santeramo on 01/10/16.                                        /
//                                                                                           /
/********************************************************************************************/
#pragma once

// System libraries
#include <algorithm>

// DACE libraries
#include "dace/dace.h"

// Project libraries
#include "SplittingHistory.h"

struct Observable;

class Patch : public DACE::AlgebraicVector<DACE::DA>
{
public:
////////////////////////////////////////////////////////////////////////////////
/*MEMBER CLASS                                                                */
////////////////////////////////////////////////////////////////////////////////
SplittingHistory history;
double t_ = 0.0;

////////////////////////////////////////////////////////////////////////////////
/*CONSTRUCTORS                                                                */
////////////////////////////////////////////////////////////////////////////////
Patch();                                                               // >! Default Constructor to create empty Patch

Patch(unsigned int size);                                              // >! Constructor with size to allocate a vector

Patch(unsigned int size, const DACE::DA &d);                                 // >! Constructor with size and elements value

Patch(const DACE::AlgebraicVector<DACE::DA> &v);                             // >! Copy constructor

Patch(const DACE::AlgebraicVector<DACE::DA> &v, const SplittingHistory &s);  // >! Copy constructor of existing DAvector and SplittingHistory


Patch(const DACE::AlgebraicVector<DACE::DA> &v, const SplittingHistory &s, double time);  // >! Copy constructor of existing DAvector and SplittingHistory

////////////////////////////////////////////////////////////////////////////////
/*OVERLOAD OPERATORS                                                          */
////////////////////////////////////////////////////////////////////////////////
Patch operator=(const DACE::AlgebraicVector<DACE::DA> &v);

////////////////////////////////////////////////////////////////////////////////
/*MEMBER FUNCTION                                                             */
////////////////////////////////////////////////////////////////////////////////
DACE::AlgebraicVector<DACE::DA> replay(DACE::AlgebraicVector<DACE::DA> obj = DACE::AlgebraicVector<DACE::DA>::identity() );

std::vector<double> getTruncationErrors();

unsigned int getSplittingDirection(const unsigned int comp);

std::pair<Patch, Patch> split( int dir = 0, DACE::AlgebraicVector<DACE::DA> obj = DACE::AlgebraicVector<DACE::DA>::identity() );

};
