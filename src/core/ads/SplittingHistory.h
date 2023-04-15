/********************************************************************************************/
//  SplittingHistory.cpp                                                                             /
//  SplittingHistory constructor and member function declaration                                      /
//                                                                                           /
//                                                                                           /
//  Created by Daniele Antonio Santeramo on 01/10/16.                                        /
//                                                                                           /
/********************************************************************************************/
#pragma once

// System libraries
#include <vector>
#include <cmath>

// DACE libraries
#include "dace/dace.h"

// Project libraries


class SplittingHistory : public std::vector<int>
{
public:
////////////////////////////////////////////////////////////////////////////////
/*CONSTRUCTORS                                                                */
////////////////////////////////////////////////////////////////////////////////
SplittingHistory();                                                                     // >! default constructor

SplittingHistory(unsigned int size);                                                    // >! Constructor with dimension gives by size

SplittingHistory(unsigned int size, const int &d);                                      // >! Constructor with dimension and equal element give by size and d

SplittingHistory(const std::vector<int> &v);                                            // >! Copy constructor of vector

SplittingHistory(const std::vector<int> &v, unsigned int first, unsigned int last);     // >! Copy constructor of

////////////////////////////////////////////////////////////////////////////////
/*OVERLOAD OPERATORS                                                          */
////////////////////////////////////////////////////////////////////////////////
friend std::ostream& operator<<(std::ostream& os, const SplittingHistory &obj);              // >! Overlod operator

////////////////////////////////////////////////////////////////////////////////
/*MEMBER FUNCTION                                                             */
////////////////////////////////////////////////////////////////////////////////
unsigned int count(unsigned int n = 0);                                                                       // >! Function to count the number of splits (default is total splits)

DACE::AlgebraicVector<DACE::DA> replay(DACE::AlgebraicVector<DACE::DA>  obj = DACE::AlgebraicVector<DACE::DA>::identity());     // >! Function to replicate the split Box

std::vector<double> center();                                                                                 // >! Function to compute the center

std::vector<double> width();                                                                                  // >! Function to compute the center

bool contain (std::vector<double> pt);                                                                        // >! Function to verify the point belonging of point

};
