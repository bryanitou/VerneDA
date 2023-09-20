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

// Project libraries
#include "base/enums.h"
#include "tools/math.h"

// DACE libraries
#include "dace/dace.h"


class SplittingHistory : public std::vector<int>
{
public:
    ////////////////////////////////////////////////////////////////////////////////
    /*CONSTRUCTORS                                                                */
    ////////////////////////////////////////////////////////////////////////////////
    /**
     * Default constructor.
     */
    SplittingHistory();                                                          // >! default constructor

    SplittingHistory(unsigned int size);                                                    // >! Constructor with dimension gives by size

    SplittingHistory(unsigned int size, const int &d);                                      // >! Constructor with dimension and equal element give by size and d

    SplittingHistory(const std::vector<int> &v);                                            // >! Copy constructor of vector

    SplittingHistory(const std::vector<int> &v, unsigned int first, unsigned int last);     // >! Copy constructor of

public:
    ////////////////////////////////////////////////////////////////////////////////
    /*OVERLOAD OPERATORS                                                          */
    ////////////////////////////////////////////////////////////////////////////////
    /**
     *
     * @param os
     * @param obj
     * @return
     */
    friend std::ostream& operator<<(std::ostream& os, const SplittingHistory &obj);

public: // Methods
    ////////////////////////////////////////////////////////////////////////////////
    /*MEMBER FUNCTION                                                             */
    ////////////////////////////////////////////////////////////////////////////////
    unsigned int count(unsigned int n = 0);                                                                       // >! Function to count the number of splits (default is total splits)

    /**
     * Function to replicate the split box: that is, the initial box split
     * @details \n
     * - obj: Where the points will be evaluated, if nothing, it is the identity (initial box) \n
     * - algorithm: Whether ADS or LOADS, so we properly split in 2 or three
     * @param obj [in] [DACE::AlgebraicVector<DACE::DA>]
     * @param algorithm [in] [ALGORITHM]
     * @return
     */
    DACE::AlgebraicVector<DACE::DA> replay(DACE::AlgebraicVector<DACE::DA>  obj = DACE::AlgebraicVector<DACE::DA>::identity(), ALGORITHM algorithm = ALGORITHM::NA);

    /**
     *
     * @param algorithm
     * @return
     */
    std::vector<double> center( ALGORITHM algorithm);                                                                                 // >! Function to compute the center

    /**
     *
     * @param algorithm
     * @return
     */
    std::vector<double> width( ALGORITHM algorithm);                                                                                  // >! Function to compute the center

    /**
     *
     * @param val
     * @return
     */
    static SPLITTING_PLACE get_splitting_place(int val);

    /**
     *
     * @param val
     * @return
     */
    static unsigned int getdir(int val);

    /**
     *
     * @param pt
     * @param algorithm
     * @return
     */
    bool contain (std::vector<double> pt, ALGORITHM algorithm);                                                                        // >! Function to verify the point belonging of point

};
