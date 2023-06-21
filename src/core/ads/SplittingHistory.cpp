/********************************************************************************************/
//  SplittingHistory.cpp                                                                             /
//  SplittingHistory constructor and member function definition                                     /
//                                                                                           /
//                                                                                           /
//  Created by Daniele Antonio Santeramo on 01/10/16.                                        /
//                                                                                           /
/********************************************************************************************/
#include "SplittingHistory.h"

////////////////////////////////////////////////////////////////////////////////
/*CONSTRUCTORS                                                                */
////////////////////////////////////////////////////////////////////////////////
SplittingHistory::SplittingHistory() : std::vector<int>() {
    /*! Default Constructor to create empty SplittingHistory object*/
}

SplittingHistory::SplittingHistory(unsigned int size) : std::vector<int>(size){
    /*! Constructor with size to allocate a vector of the given size with elements initialized using their default constructor.
       \param[in] size length of SplittingHistory.
     */
}

SplittingHistory::SplittingHistory(unsigned int size, const int &d) : std::vector<int>(size, d){
    /*! Constructor with size and elements value to allocate a vector of the given size with elements initialized as copies of d.
       \param[in] size length of SplittingHistory
       \param[in] d  initial value for the elements
     */
}

SplittingHistory::SplittingHistory(const std::vector<int> &v) : std::vector<int>(v){
    /*! Copy constructor to create a copy of any existing vector.
      \param[in] v vector to be copied into PatchVector
    */
}

SplittingHistory::SplittingHistory(const std::vector<int> &v, unsigned int first, unsigned int last) : std::vector<int>(v.begin()+first, v.begin()+last+1){
    /*! Extraction constructor to copy only a given range of elements from vector v.
       \param[in] v vector to be copied into PatchVector
       \param[in] first index of the first element to be copied
       \param[in] last index of the last element to be copied
       \note The constructor does not perform any range checking for the extraction.
     */
        // Notice that the range in the std::vector constructor above includes all
        // elements between first and last, including the first excluding the last.
        // Hence the +1.
}

template <typename T> int sgn(T val)
{
    return (T(0) < val) - (val < T(0));
}

////////////////////////////////////////////////////////////////////////////////
/*OVERLOAD OPERATORS                                                          */
////////////////////////////////////////////////////////////////////////////////
std::ostream& operator<<(std::ostream& os, const SplittingHistory &obj) {

    const unsigned int size = obj.size();

    os << "[[[ " << size << " vector" << "\n";
    for(unsigned int i=0; i<size;i++){
        os << obj[i] << "\t";}
        os << "\n" << "]]]" << "\n";

    return os;
}

////////////////////////////////////////////////////////////////////////////////
/*MEMBER FUNCTION                                                             */
////////////////////////////////////////////////////////////////////////////////
unsigned int SplittingHistory::count(unsigned int n)
{
    /* member fuction to compute the number of splits in varius direction
       INPUT  param[in] the splitting history vector is the hidden object of function
              int n: splitting direction indicator  n=0 total number of splits
                                                    n=1,2... number of splits along the 1,2... variable direction
        OUTPUT int: number of splits*/
    if ( n == 0) {return this -> size();}
    else {
        unsigned int c = 0;
        for ( int i = 0; i < this -> size(); ++i) {
            if ( abs(this -> at(i)) == n ) { c += 1;}
    }

      return c;
    }
}


DACE::AlgebraicVector<DACE::DA> SplittingHistory::replay(DACE::AlgebraicVector<DACE::DA> obj, ALGORITHM algorithm)
{
    /*member function to replicate the box splitted by means the assigned splitting history
    INPUT param[in]: the splitting history vector is the hidden object of function
          AlgebraicVector<DA> obj: DAvector of initial Domain
    OUTPUT return the AlgebraicVector <DA> of box corresponding to splitting history
    */
    DACE::AlgebraicVector<DACE::DA> x = DACE::AlgebraicVector<DACE::DA>::identity();
    const unsigned int size = this -> size();

    for ( unsigned int i = 0; i < size; ++i)
    {
        // Get the direction in which it was split and if left or right or center
        auto splitting_val = (*this)[i];

        // Get the splitting direction
        unsigned int dir = SplittingHistory::getdir(splitting_val);

        // Get the splitting position
        unsigned int n = dir - 1;

        // Get the sign as double
        auto sign = (double)(tools::math::sgn(splitting_val));

        // Shifting
        auto shift = SplittingHistory::get_splitting_place(splitting_val) == SPLITTING_PLACE::MIDDLE ? 0.0 :  (algorithm == ALGORITHM::LOADS ? 1.0/3.0 : 0.5 ) * sign ;

        // Shift and scale
        x[n] = shift + (algorithm == ALGORITHM::LOADS ? 1.0/3.0 : 0.5 ) * DACE::DA(n+1);

        // Compose
        obj = obj.eval(x);

        x[n] = DACE::DA(n + 1);
    }

    return obj;
}

/**
 *      int n = abs(this -> at(i)) - 1;
        x[n] = 0.5*(double)((*this)[i]/abs((*this)[i])) + 0.5*DACE::DA(n+1);
        obj = obj.eval(x);
        x[n] = DACE::DA(n+1);
 * @return
 */

std::vector<double> SplittingHistory::center(ALGORITHM algorithm)
{
    /*member function to compute the center of the box after the splitting represented by the splitting history vector
    INPUT param[in] the splitting history vector is the hidden object of function
          AlgebraicVector<DA> obj: DAvector of initial Domain
    OUTPUT return a vector conteins the resulting box center with respect of the splitting history vector considered
    */

    // Get number of variables
    const unsigned int var = DACE::DA::getMaxVariables();

    // Check this size: splitting history: That is, the information on how this patch has been partitioned
    const unsigned int size =  this->size();

    // Auxiliary variable for the initial center and width
    std::vector <double> c(var, 0.0), w(var, 2.0);

    for ( unsigned int i = 0; i < size; ++i)
    {
        // Get the direction in which it was split and if left or right or center
        auto splitting_val = (*this)[i];

        // Get the splitting direction
        unsigned int dir = SplittingHistory::getdir(splitting_val);

        // Get the splitting position
        unsigned int n = dir - 1;

        // Get the sign as double
        auto sign = (double)(tools::math::sgn(splitting_val));

        // Shift center // TODO: Make a choice here LOADS vs. ADS.
        if (algorithm == ALGORITHM::LOADS)
        {
            c[n] = c[n] + ((SPLITTING_PLACE::MIDDLE == get_splitting_place(splitting_val)) ? 0.0 : 1.0/3.0 * sign * w[n]);
        }
        else
        {
            c[n] = c[n] + 0.5*0.5 * sign * w[n];
        }

        // Re-scale width
        w[n] = (algorithm == ALGORITHM::LOADS ? 1.0/3.0 : 0.5 ) * w[n];
    }

      return c;
}

std::vector<double> SplittingHistory::width(ALGORITHM algorithm)
{
    /* member function to compute the width of the box after the splitting represented by the splitting history vector
    INPUT param[in] the splitting history vector is the hidden object of function
          AlgebraicVector<DA> obj: DAvector of initial Domain
    OUTPUT return a vector conteins the resulting box width with respect of the splitting history vector considered
    */
    const unsigned int var = DACE::DA::getMaxVariables();
    const unsigned int size = this -> size();
    std::vector <double> w(var, 2.0);

    for (unsigned int i = 0; i < size; ++i)
    {
        // Get the direction
        auto dir = SplittingHistory::getdir((*this)[i]);

        // TODO: MAKE A CHOICE HERE
        w[dir - 1] = (algorithm == ALGORITHM::LOADS ? 1.0/3.0 : 0.5 ) * std::fabs(w[dir -1]);
    }

    return w;
}


bool SplittingHistory::contain(std::vector<double> pt, ALGORITHM algorithm)
{
    /* member function to know if a point belong to an assigned box
    INPUT pt: vector conteining the point to check the belonging
              param[in] the splitting history vector is the hidden object of function
              AlgebraicVector<DA> obj: DAvector of initial Domain !>it is possible to assign a specific box where check the belonging of the point
                                                                    in this case it is not needed to assign the param[in] but to call only the function<!
    OUTPUT the function check if the point digit has the same dimension of set box and, in positive case, return the belonging or not of the point
           to the set box assigning -> 1: the point is contained; 0: the point is not contained
     */

    // Get the amount of variables used...
    const unsigned int var = DACE::DA::getMaxVariables();

    // Safety check size
    if (var != pt.size())
    {
        throw std::runtime_error ("error in 'splitting_history::contain': The dimension of selected point is wrong, select a new point with rigth dimension");
    }
    else
    {
        // Get the center and direction
        auto c = SplittingHistory::center(algorithm);
        auto w = SplittingHistory::width(algorithm);

        // Check the point is lying within the expected domain
        for(unsigned int i = 0; i < pt.size(); ++i)
        {
            // Get the distance to the center
            auto dist = std::fabs(pt[i] - c[i]);

            // Get half of the width
            auto half_w =  0.5 * w[i];

            // Check the distance is within the box
            if (dist > half_w)
            {
                // If violated, exit function returning false
                return false;
            }
      }
    }

    // Everything is alright
    return true;
}

SPLITTING_PLACE SplittingHistory::get_splitting_place(int val)
{
    // Get the direction in which it was splitted and if left or right or center
    return val < 0 ? SPLITTING_PLACE::LEFT : val < 100 ? SPLITTING_PLACE::RIGHT : SPLITTING_PLACE::MIDDLE;
}

unsigned int SplittingHistory::getdir(int val)
{
    // Get the direction from the place
    return SplittingHistory::get_splitting_place(val) == SPLITTING_PLACE::MIDDLE ? val/100 :  std::abs(val);
}
