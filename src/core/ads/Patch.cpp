/********************************************************************************************/
//  Patch.cpp                                                                             /
//  Patch constructor and member function definition                                      /
//                                                                                           /
//                                                                                           /
//  Created by Daniele Antonio Santeramo on 01/10/16.                                        /
//                                                                                           /
/********************************************************************************************/

#include "Patch.h"

struct Observable;

////////////////////////////////////////////////////////////////////////////////
/*MEMBER CLASS                                                                */
////////////////////////////////////////////////////////////////////////////////
SplittingHistory history;

////////////////////////////////////////////////////////////////////////////////
/*CONSTRUCTORS                                                                */
////////////////////////////////////////////////////////////////////////////////
Patch::Patch() : DACE::AlgebraicVector<DACE::DA>(){
    /*! Default Constructor to create empty Patch*/
}

Patch::Patch(unsigned int size) : DACE::AlgebraicVector<DACE::DA>(size){
    /*! Constructor with size to allocate a vector of the given size with elements initialized using their default constructor.
       \param[in] size length of Patch DAvector.
     */
}

Patch::Patch(unsigned int size, const DACE::DA &d) : DACE::AlgebraicVector<DACE::DA>(size, d){
    /*! Constructor with size and elements value to allocate a vector of the given size with elements initialized as copies of d.
       \param[in] size length of PatchVector
       \param[in] d    initial value for the elements
     */
}

Patch::Patch(const DACE::AlgebraicVector<DACE::DA> &v) : DACE::AlgebraicVector<DACE::DA>(v){
    /*! Copy constructor to create a copy of any existing DAvector.
       \param[in] v DAvector to be copied into Patch DAvector
     */
    history = std::vector<int>();
}

Patch::Patch(const DACE::AlgebraicVector<DACE::DA> &v, const SplittingHistory &s) : DACE::AlgebraicVector<DACE::DA>(v){

    /*! Copy constructor to create a copy of any existing DAvector and SplittingHistory.
       \param[in] v DAvector and s SplittingHistory to be copied into Patch
     */
    history = s;
}

////////////////////////////////////////////////////////////////////////////////
/*OVERLOAD OPERATORS                                                          */
////////////////////////////////////////////////////////////////////////////////
Patch Patch::operator=(const DACE::AlgebraicVector<DACE::DA> &v) {
    /*Operator to equal a DAvector to a Patch element*/
    this -> clear();

    const unsigned int size = v.size();

    for (int i = 0; i < size; ++i){
        this -> push_back(v[i]);
    }

    return (*this);
}

////////////////////////////////////////////////////////////////////////////////
/*MEMBER FUNCTION                                                             */
////////////////////////////////////////////////////////////////////////////////
DACE::AlgebraicVector<DACE::DA> Patch::replay(DACE::AlgebraicVector<DACE::DA> obj )
{
    /*it is a way to call the 'replay' function with a Patch element
    AlgebraicVector<DA> obj: DAvector of initial Domain
    return: the function call the SplittingHistory replay*/

    return history.replay(obj);
}

std::vector<double> Patch::getTruncationErrors()
{
    /*member function to exstimate the maximum error of an AlgebraicVector expanded over a domain
    this member function is allowed to call by means a Patch element
    \param[in] the hidden input is the Patch element of polynomial expansion
    output -> return a vector of double contain: the value of maximum order error for each component*/

    unsigned const int size = this -> size();
    std::vector<double> Errors;

    for ( unsigned int i = 0; i < size; ++i) {
        auto err = (*this)[i].estimNorm(0, 0, DACE::DA::getMaxOrder() + 1);
        Errors.push_back(err[err.size()-1]);
    }

    return Errors;
    }


unsigned int Patch::getSplittingDirection(const unsigned int comp)
{
    /*member function to calculate the splitting direction
      this constructor is allowed to call by means a Patch element
      \param[in] the hidden input is the Patch element
      int comp: is the component of function DAvector with maximum error
      output -> return a 'int' of splitting direction*/
    unsigned int dir = 0;
    double exd = 0;
    for ( unsigned int i = 0; i < DACE::DA::getMaxVariables(); ++i ) {
        auto err = (*this)[comp].estimNorm(i+1, 0, DACE::DA::getMaxOrder() + 1);
        double errM = err[err.size()-1];
        dir = errM > exd ? i+1 : dir;
        exd = errM > exd ? errM : exd;
    }

    return dir;
}


std::pair<Patch, Patch> Patch::split( int dir, DACE::AlgebraicVector<DACE::DA> obj )
{
    /* member function to split the Patch
       \param[in] the hidden input is the 'Patch'
       int comp: is the component of function DAvector with maximum error
       int dir: is the splitting direction
       return a std::pair containing the two Patch obtained by splitting*/
    if ( dir == 0) {
        /*it is possible to use the automatic splitting direction computation if only if the Truncation error is evaluated on the domain itself
        and not if exist a function expanded on the domain, in this last case the direction MUST be compute outside the Patch class and gives as input */
        auto errors = this -> Patch::getTruncationErrors();
        int pos = std::distance(errors.begin(), std::max_element(errors.begin(), errors.end()) );
        dir = Patch::getSplittingDirection(pos);
    }

    std::pair<Patch, Patch> output;
    Patch temp = (*this);

    temp.history.push_back( -dir );
    obj[dir-1] =  -0.5 + 0.5*DACE::DA(dir);
    temp = this -> eval(obj);
    output.first = temp;
    temp.history.pop_back();


    temp.history.push_back( dir );
    obj[dir-1] =  0.5 + 0.5*DACE::DA(dir);
    temp = this -> eval(obj);
    output.second = temp;
    temp.history.pop_back();

    return output;
}
