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
    this->history = std::vector<int>();
}

Patch::Patch(const DACE::AlgebraicVector<DACE::DA> &v, const SplittingHistory &s) : DACE::AlgebraicVector<DACE::DA>(v)
{
    /*! Copy constructor to create a copy of any existing DAvector and SplittingHistory.
       \param[in] v DAvector and s SplittingHistory to be copied into Patch
     */
    history = s;
}

Patch::Patch(const DACE::AlgebraicVector<DACE::DA> &v, const SplittingHistory &s, const std::vector<double> &t,const std::vector<double> &nlis, ALGORITHM algorithm, double time, double nli, double time_split) : DACE::AlgebraicVector<DACE::DA>(v)
{
    /*! Copy constructor to create a copy of any existing DAvector and SplittingHistory.
       \param[in] v DAvector and s SplittingHistory to be copied into Patch
     */
    this->history = s;
    this->times = t;
    this->t_ = time;
    this->nli = nli;
    this->nlis = nlis;
    this->t_split_ = time_split;
    this->algorithm_ = algorithm;

    // Set some constants
    this->scaling = ALGORITHM::LOADS == this->algorithm_ ? 1.0/3.0 : 0.5;
    this->center = ALGORITHM::LOADS == this->algorithm_ ? 2.0/3.0 : 0.5;
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

    return history.replay(this->algorithm_, obj);
}

std::vector<double> Patch::getTruncationErrors()
{
    /* Member function to estimate the maximum error of an AlgebraicVector expanded over a domain
    this member function is allowed to call by means a Patch element
    \param[in] the hidden input is the Patch element of polynomial expansion
    output -> return a vector of double contain: the value of maximum order error for each component*/

    // Auxiliary variables
    unsigned const int size = this->size();
    unsigned int n_max_ord = DACE::DA::getMaxOrder();

    // Result to be obtained
    std::vector<double> Errors;

    // Iterate through the size of the patch
    for (unsigned int i = 0; i < size; ++i)
    {
        // Get the DA variable where we will extract the error
        auto da2eval = (*this)[i];

        // Get the errors vector associated with this variable
        auto err = da2eval.estimNorm(0, 0, n_max_ord + 1);

        // Push back last element of the err vector
        Errors.push_back(err.back());
    }

    // Return the errors
    return Errors;
}


unsigned int Patch::getSplittingDirection(const unsigned int comp)
{
    return Patch::getSplittingDirection(comp, DACE::AlgebraicVector<DACE::DA>(*this));
}

unsigned int Patch::getSplittingDirection(const unsigned int comp, DACE::AlgebraicVector<DACE::DA> algebraicVector)
{
    /** Member function to calculate the splitting direction this constructor is allowed to call by means a Patch element
      * \param[in]: the hidden input is the Patch element.
      * int comp: is the component of function DA vector with maximum error
      * output -> return a 'int' of splitting direction
      **/

    unsigned int dir = 0;
    double exd = 0;

    // Auxiliary variables
    unsigned int n_var = DACE::DA::getMaxVariables();
    unsigned int n_max_ord = DACE::DA::getMaxOrder();

    // Get the patch which is supposed to have the maximum error
    auto da2eval_max_err = algebraicVector[comp];

    // Iterate 'n_var' times
    for (unsigned int i = 0; i < n_var; ++i )
    {
        // Get the errors vector associated with this variable
        auto err = da2eval_max_err.estimNorm(i + 1, 0, n_max_ord + 1);

        double errM = err[err.size()-1];
        dir = errM > exd ? i+1 : dir;
        exd = errM > exd ? errM : exd;
    }

    return dir;
}


std::vector<Patch> Patch::split(int dir, DACE::AlgebraicVector<DACE::DA> obj)
{
    /*
     * Member function to split the Patch \param[in] the hidden input is the 'Patch'
     * int comp: is the component of function DA vector with maximum error
     * int dir: is the splitting direction
     * return a std::pair containing the two Patch obtained by splitting
     */

    if ( dir == 0)
    {
        /*
         * It is possible to use the automatic splitting direction computation if only if the Truncation error is
         * evaluated on the domain itself and not if exist a function expanded on the domain, in this last case the
         * direction MUST be compute outside the Patch class and gives as input
         */

        auto errors = this -> Patch::getTruncationErrors();
        int pos = std::distance(errors.begin(), std::max_element(errors.begin(), errors.end()) );
        dir = Patch::getSplittingDirection(pos);
    }

    std::vector<Patch> output(ALGORITHM::LOADS == this->algorithm_ ? 3 : 2);
    Patch temp = (*this);

    temp.history.push_back( -dir );
    obj[dir-1] = -this->center + this->scaling * DACE::DA(dir);
    temp = this -> eval(obj);

    // Save time
    temp.times.push_back(this->t_);
    temp.nlis.push_back(this->nli);

    // Make copy
    output[0] = temp;

    // Clear last position
    temp.history.pop_back();
    temp.times.pop_back();
    temp.nlis.pop_back();

    temp.history.push_back( dir );
    obj[dir-1] = +this->center + this->scaling * DACE::DA(dir);
    temp = this -> eval(obj);

    // Save time
    temp.times.push_back(this->t_);
    temp.nlis.push_back(this->nli);

    // Make copy
    output[1] = temp;

    // Clear last position
    temp.history.pop_back();
    temp.times.pop_back();
    temp.nlis.pop_back();

    // In case of having loads, input the scaled centered patch
    if (ALGORITHM::LOADS == this->algorithm_)
    {
        // Set temporal patch
        temp.history.push_back( dir * 100);
        obj[dir-1] = 0.0 + scaling * DACE::DA(dir);
        temp = this -> eval(obj);

        // Save time
        temp.times.push_back(this->t_);
        temp.nlis.push_back(this->nli);

        // Make copy
        output[2] = temp;

        // Clear last position
        temp.history.pop_back();
        temp.times.pop_back();
        temp.nlis.pop_back();
    }

    return output;
}
