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
#include <utility>

// Project libraries
#include "SplittingHistory.h"

struct Observable;

class Patch : public DACE::AlgebraicVector<DACE::DA>
{
public:
    ////////////////////////////////////////////////////////////////////////////////
    /*MEMBER CLASS                                                                */
    ////////////////////////////////////////////////////////////////////////////////
    double t_ = 0.0;
    double nli = 0.0;
    double t_split_ = -1;
    int id_ = 0;
    ALGORITHM algorithm_{ALGORITHM::NONE};

    // Auxiliary variables
    double scaling;
    double center;

private: // Attributes

    // Stores the history of the patch by integer information (how every patch has been partitioned along the time)
    SplittingHistory history;

    // Stores the history of the patch by its time and NLI
    std::vector<double> times;
    std::vector<double> nlis;

public:
    // TODO: Make it private
    std::vector<double> betas;

public:
    ////////////////////////////////////////////////////////////////////////////////
    /*CONSTRUCTORS                                                                */
    ////////////////////////////////////////////////////////////////////////////////
    Patch();                                                               // >! Default Constructor to create empty Patch

    Patch(unsigned int size);                                              // >! Constructor with size to allocate a vector

    Patch(unsigned int size, const DACE::DA &d);                                 // >! Constructor with size and elements value

    Patch(const DACE::AlgebraicVector<DACE::DA> &v);                             // >! Copy constructor

    Patch(const DACE::AlgebraicVector<DACE::DA> &v, const SplittingHistory &s);  // >! Copy constructor of existing DAvector and SplittingHistory


    Patch(const DACE::AlgebraicVector<DACE::DA> &v, const SplittingHistory &s, const std::vector<double> &t, const std::vector<double> &nlis, ALGORITHM algorithm, double time, double nli = -1, double time_split = -1);  // >! Copy constructor of existing DAvector and SplittingHistory

    ////////////////////////////////////////////////////////////////////////////////
    /*OVERLOAD OPERATORS                                                          */
    ////////////////////////////////////////////////////////////////////////////////
    Patch operator=(const DACE::AlgebraicVector<DACE::DA> &v);

    ////////////////////////////////////////////////////////////////////////////////
    /*MEMBER FUNCTION                                                             */
    ////////////////////////////////////////////////////////////////////////////////
    DACE::AlgebraicVector<DACE::DA> replay(DACE::AlgebraicVector<DACE::DA> obj = DACE::AlgebraicVector<DACE::DA>::identity() );

    std::vector<double> getTruncationErrors();

    std::vector<Patch>  split( int dir = 0, DACE::AlgebraicVector<DACE::DA> obj = DACE::AlgebraicVector<DACE::DA>::identity());

    unsigned int getSplittingDirection(unsigned int comp);

    static unsigned int getSplittingDirection(unsigned int comp, AlgebraicVector <DACE::DA> algebraicVector);


    ////////////////////////////////////////////////////////////////////////////////
    /*HISTORY WRAPPER                                                             */
    ////////////////////////////////////////////////////////////////////////////////
    auto history_is_empty() { return this->history.empty(); }
    auto get_history_int() {return (std::vector<int>)this->history;}
    auto get_history_count(int n = 0) {return (int)this->history.count(n); }
    auto history_contains(std::vector<double> pt) { return this->history.contain(std::move(pt), this->algorithm_); }
    auto get_center() { return this->history.center(this->algorithm_); }
    auto get_width() { return this->history.width(this->algorithm_); }

    ////////////////////////////////////////////////////////////////////////////////
    /*TIMES WRAPPER                                                             */
    ////////////////////////////////////////////////////////////////////////////////
    auto get_times_doubles() {return this->times;}
    auto get_nlis_doubles() {return this->nlis;}
};

