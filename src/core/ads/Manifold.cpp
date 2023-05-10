/********************************************************************************************/
//  Manifold.cpp                                                                             /
//  Manifold constructor and member function definition                                      /
//                                                                                           /
//                                                                                           /
//  Created by Daniele Antonio Santeramo on 01/10/16.                                        /
//                                                                                           /
/********************************************************************************************/

#include "Manifold.h"

struct Observable;

Manifold::Manifold() : std::deque< Patch >()
{

}

Manifold::Manifold( unsigned int n ) : std::deque< Patch >(n)
{

}

Manifold::Manifold( const Manifold& m) : std::deque< Patch >(m)
{

}

Manifold::Manifold( const Patch& p)
{
    // Create a temporal Manifold
    Manifold temp;

    // Push back the input patch to a temporal Manifold
    temp.push_back(p);

    // TODO: WTF: Set the created temporal Manifold to "this" class instance
    *this = temp;

}

Manifold::Manifold( const DACE::AlgebraicVector<DACE::DA>& p)
{
    // Create a patch from the DA vector
    Patch temp = p;

    // TODO: WTF: Create Manifold from a Patch
    Manifold aux(temp);

    // TODO: WTF: Set aux to "this" class instance from a temporal Patch
    *this = aux;
}

Manifold Manifold::getSplitDomain(DACE::AlgebraicVector<DACE::DA> (*func)(DACE::AlgebraicVector<DACE::DA> ), const double errToll, const int nSplitMax, int posOverride) {
  /* Member function elaborating the ADS of initial Manifold (initial Domain)
  !>> input: (*func) is the expansion function whose error is estimate
             double errToll is the threshold for the estimation error
             double nSplitMax is the threshold for the maximun number of split for each box
   !<< return Manifold containing the generating Patch */

    /*automatic Domain splitting*/

    Manifold results;

    /*execute steps of automatic domain splitting, calling the function to eximate the error and to split the Patch*/
    while ( !this -> empty() ) {

        Patch p = this -> front();
        this -> pop_front();

        Patch f(func(p), p.history);

        /*Estimate the error for each patch in the deque */
        auto error = f.getTruncationErrors();
        std::vector<double>::iterator max_error = std::max_element(error.begin(), error.end());   // find maximum truncation error
        if ( posOverride != 0) max_error = (error.begin() + posOverride);

        if ( *max_error < errToll || p.history.count() == nSplitMax) { //check the maximum function error and the total number of split for the Patch
            results.push_back(f);
        }
        else {
            const unsigned int pos = std::distance(error.begin(), max_error);  // function component of maximum error
            const unsigned int dir = f.getSplittingDirection(pos);
            auto s = p.split(dir);
            this -> push_back(s.first);
            this -> push_back(s.second);
        }

    }

    return results;
}

Manifold Manifold::getSplitDomain(DACE::AlgebraicVector<DACE::DA> (*func)(DACE::AlgebraicVector<DACE::DA>, double ), const double errtol, const int nSplitMax, const double mu, int posOverride) {
  /* Member function elaborating the ADS of initial Manifold (initial Domain)
  !>> input: (*func) is the expansion function whose error is estimate
             double errtol is the threshold for the estimation error
             double nSplitMax is the threshold for the maximun number of split for each box
   !<< return Manifold containing the generating Patch */

    /*automatic Domain splitting*/

    Manifold results;

    /*execute steps of automatic domain splitting, calling the function to eximate the error and to split the Patch*/
    while ( !this -> empty() ) {

        Patch p = this -> front();
        this -> pop_front();

        Patch f(func(p, mu), p.history);

        /*Estimate the error for each patch in the deque */
        auto error = f.getTruncationErrors();
        std::vector<double>::iterator max_error = std::max_element(error.begin(), error.end());   // find maximum truncation error
        if ( posOverride != 0) max_error = (error.begin() + posOverride);


        if ( *max_error < errtol || p.history.count() == nSplitMax) { //check the maximum function error and the total number of split for the Patch
            results.push_back(f);
        }
        else {
            const unsigned int pos = std::distance(error.begin(), max_error);  // function component of maximum error
            const unsigned int dir = f.getSplittingDirection(pos);
            auto s = p.split(dir);
            this -> push_back(s.first);
            this -> push_back(s.second);
        }

    }

    return results;
}

Manifold Manifold::getSplitDomain(DACE::AlgebraicVector<DACE::DA> (*func)(DACE::AlgebraicVector<DACE::DA>, double), const std::vector<double> errToll, const int nSplitMax, const double mu, int posOverride) {
    /* Member function elaborating the ADS of initial Manifold (initial Domain)
     !>> input: (*func) is the expansion function whose error is estimate
     std::vector<double> errToll is the threshold for the estimation error for aech component of Patch
     double nSplitMax is the threshold for the maximun number of split for each box
     !<< return Manifold containing the generating Patch */
    
    /*automatic Domain splitting*/
    
    Manifold results;
    
    /*execute steps of automatic domain splitting, calling the function to eximate the error and to split the Patch*/
    while ( !this -> empty() ) {
        
        Patch p = this -> front();
        this -> pop_front();
        
        Patch f(func(p, mu), p.history);
        if ( errToll.size() != f.size() ) throw std::runtime_error ("Error in Manifold::getSplitDomain: The Tollerance vector must have the same size of Patchs ");
        
        /*Estimate the error for each patch in the deque */
        std::vector<double> error = f.getTruncationErrors();
        std::vector<double> relativErr(error.size() );
        
        for ( unsigned int k = 0; k < relativErr.size(); ++k ) {
            relativErr[k] = error[k] - errToll[k];
        }
        for ( unsigned int k = 0; k < relativErr.size(); ++k ) {
            if (error[k] > errToll[k]) relativErr[k] = DACE::abs(relativErr[k]);
            else relativErr[k] = 0.0;
        }
        
        std::vector<double>::iterator max_error = std::max_element(relativErr.begin(), relativErr.end() );   // find maximum relative truncation error
        if ( posOverride != 0) max_error = (relativErr.begin() + posOverride);
        
        if (  *max_error == 0.0 || p.history.count() == nSplitMax) { //check the maximum function error and the total number of split for the Patch
            results.push_back(f);
        }
        else {
            const unsigned int pos = std::distance(relativErr.begin(), max_error);  // function component of maximum error
            const unsigned int dir = f.getSplittingDirection(pos);
            auto s = p.split(dir);
            this -> push_back(s.first);
            this -> push_back(s.second);
        }
        
    }
    
    return results;
}


void Manifold::print_status()
{
    std::string msg2write = tools::string::print2string("TRACE: Manifold size: %10d", this->size());

    // Print msg
    std::fprintf(stdout, "%s\n", msg2write.c_str());
}


Manifold* Manifold::getSplitDomain(const std::vector<double>& errToll, const int nSplitMax, int posOverride)
{
    /* Member function elaborating the ADS of initial Manifold (initial Domain)
     !>> input: (*func) is the expansion function whose error is estimate
     std::vector<double> errToll is the threshold for the estimation error for each component of Patch
     double nSplitMax is the threshold for the maximum number of split for each box
     !<< return Manifold containing the generating Patch */
    
    /*Automatic Domain Splitting*/
    auto results = new Manifold();
    
    /*execute steps of Automatic Domain Splitting, calling the function to estimate the error and to split the Patch*/
    // While runs until the vector gets emptied >> (std::deque< Patch >)
    while (!this->empty())
    {
        // Print status
        this->print_status();

        // Creates new patch from the first position in this Manifold
        Patch p = this->front();

        // Removes the one in front
        this->pop_front();

        // Set time for the integrator
        this->integrator_->t_ = p.t_;

        // Get the new state
        auto scv = this->integrator_->integrate(p, (int)this->size());

        // Builds patch from the resulting scv
        Patch f(scv, p.history, this->integrator_->t_);

        // Check for tolerance
        // TODO: This check needs to be done only once
        if (errToll.size() != f.size())
        {
            throw std::runtime_error ("Error in Manifold::getSplitDomain: The Tolerance vector must have the same "
                                      "size of Patchs.");
        }
        
        /*Estimate the error for each patch in the deque */
        std::vector<double> error = f.getTruncationErrors();
        std::vector<double> relativErr(error.size() );
        
        for ( unsigned int k = 0; k < relativErr.size(); ++k )
        {
            relativErr[k] = error[k] - errToll[k];
        }
        for ( unsigned int k = 0; k < relativErr.size(); ++k )
        {
            if (error[k] > errToll[k])
            {
                relativErr[k] = DACE::abs(relativErr[k]);
            }
            else
            {
                relativErr[k] = 0.0;
            }
        }

        // Find maximum relative truncation error
        auto max_error = std::max_element(relativErr.begin(), relativErr.end());

        // If
        if ( posOverride != 0)
        {
            max_error = (relativErr.begin() + posOverride);
        }
        
        if (*max_error == 0.0 || p.history.count() == nSplitMax)
        {
            // Check the maximum function error and the total number of split for the Patch
            results->push_back(f);
        }
        else
        {
            // Function component of maximum error
            const unsigned int pos = std::distance(relativErr.begin(), max_error);

            // Get the splitting direction
            const unsigned int dir = f.getSplittingDirection(pos);

            // Split the patch
            // TODO: Debug why it SIGSEVs here
            auto s = p.split(dir);

            // Add new manifolds
            this->push_back(s.first);
            this->push_back(s.second);
        }
    }
    
    return results;
}


Manifold Manifold::getSplitDomain(DACE::AlgebraicVector<DACE::DA> (*func)(DACE::AlgebraicVector<DACE::DA>, Observable*, double), const double errToll, const int nSplitMax, Observable* param, const double mu, int posOverride) {
  /* Member function elaborating the ADS of initial Manifold (initial Domain)
  !>> input: (*func) is the expansion function whose error is estimate
             double errToll is the threshold for the estimation error
             double nSplitMax is the threshold for the maximun number of split for each box
   !<< return Manifold containing the generating Patch */

    /*automatic Domain splitting*/

    Manifold results;

    /*execute steps of automatic domain splitting, calling the function to eximate the error and to split the Patch*/
    while ( !this -> empty() ) {

        Patch p = this -> front();
        this -> pop_front();

        Patch f(func(p, param, mu), p.history);

        /*Estimate the error for each patch in the deque */
        auto error = f.getTruncationErrors();
        std::vector<double>::iterator max_error = std::max_element(error.begin(), error.end());   // find maximum truncation error
        if ( posOverride != 0) max_error = (error.begin() + posOverride);

        if ( *max_error < errToll || p.history.count() == nSplitMax) { //check the maximum function error and the total number of split for the Patch
            results.push_back(f);
        }
        else {
            const unsigned int pos = std::distance(error.begin(), max_error);  // function component of maximum error
            const unsigned int dir = f.getSplittingDirection(pos);
            auto s = p.split(dir);
            this -> push_back(s.first);
            this -> push_back(s.second);
        }

    }

    return results;
}

Manifold Manifold::getSplitDomain(DACE::AlgebraicVector<DACE::DA> (*func)(DACE::AlgebraicVector<DACE::DA>, Observable*, double), const std::vector<double> errToll, const int nSplitMax, Observable* param, const double mu, int posOverride) {
  /* Member function elaborating the ADS of initial Manifold (initial Domain)
  !>> input: (*func) is the expansion function whose error is estimate
             std::vector<double> errToll is the threshold for the estimation error for aech component of Patch
             double nSplitMax is the threshold for the maximun number of split for each box
   !<< return Manifold containing the generating Patch */

    /*automatic Domain splitting*/

    Manifold results;

    /*execute steps of automatic domain splitting, calling the function to eximate the error and to split the Patch*/
    while ( !this -> empty() ) {

        Patch p = this -> front();
        this -> pop_front();

        Patch f(func(p, param, mu), p.history);
        if ( errToll.size() != f.size() ) throw std::runtime_error ("Error in Manifold::getSplitDomain: The Tollerance vector must have the same size of Patchs ");

        /*Estimate the error for each patch in the deque */
        std::vector<double> error = f.getTruncationErrors();
        std::vector<double> relativErr(error.size() );

        for ( unsigned int k = 0; k < relativErr.size(); ++k ) {
            relativErr[k] = error[k] - errToll[k];
        }
        for ( unsigned int k = 0; k < relativErr.size(); ++k ) {
            if (error[k] > errToll[k]) relativErr[k] = DACE::abs(relativErr[k]);
            else relativErr[k] = 0.0;
        }

        std::vector<double>::iterator max_error = std::max_element(relativErr.begin(), relativErr.end() );   // find maximum relative truncation error
        if ( posOverride != 0) max_error = (relativErr.begin() + posOverride);

        if (  *max_error == 0.0 || p.history.count() == nSplitMax) { //check the maximum function error and the total number of split for the Patch
            results.push_back(f);
        }
        else {
            const unsigned int pos = std::distance(relativErr.begin(), max_error);  // function component of maximum error
            const unsigned int dir = f.getSplittingDirection(pos);
            auto s = p.split(dir);
            this -> push_back(s.first);
            this -> push_back(s.second);
        }

    }

    return results;
}

DACE::AlgebraicVector<double> Manifold::pointEvaluationManifold( DACE::AlgebraicVector<DACE::DA> InitSet, DACE::AlgebraicVector<double> pt, const int flag)
{
  /* Member function elaborating the Manifold after split
  !>> input: AlgebraicVector<DA> InitSet: is the initial Manifold (initial Domain)
             AlgebraicVector<double> pt: is a point (absolutly) belonging to Initial Domain taken into account
             flag == 0 for point scalede between [-1,1], flag == 1 for real point
             VarDom: number of variable actually used as domain variable
   !<< return AlgebraicVector<DA> of the expansion of the Patch(the sub-domain after the ADS elaboration) which contain the point assigned  */
    //std::cout << pt << std::endl;
    if ( pt.size() != DACE::DA::getMaxVariables())
    {
        throw std::runtime_error ("error in 'Manifold::pointEvaluationManifold': The dimension of selected point is wrong, the dimension must be the same of Patch variables");
    }

    if ( flag == 0 )
    {
        SplittingHistory empty;

        if (!empty.contain(pt) )
        {
            throw std::runtime_error ("error in 'Manifold::pointEvaluationManifold': The selected point seems to be outside the initial Domain");
        }

        const unsigned int size = this -> size();
        for ( unsigned int i = 0; i < size; ++i )
        {

            if ( (*this).at(i).history.contain(pt) )
            {

                DACE::AlgebraicVector<double> c((*this).at(i).history.center() );
                DACE::AlgebraicVector<double> w((*this).at(i).history.width() );
                DACE::AlgebraicVector<double> ptPatch = 2.0*(pt - c)/w;
                return this -> at(i).eval(ptPatch);
            }
            /*In case that the manifold is been modified, i.e delate some Patch,
            and the point belongs to a delated Patch, the member function riturn
            a NaN vector ! ATTENTION*/
            if ( i == size-1 )
            {
                DACE::AlgebraicVector<double> Null(this -> at(0).size(), NAN );
                return Null;
            }
        }
    }

    if ( flag == 1 ) {

        const unsigned int comp = DACE::DA::getMaxVariables();

        DACE::AlgebraicVector<double> ptUnit(comp), cnt(comp);
        DACE::AlgebraicVector<double> w(comp, 2.0 );

        cnt = InitSet.cons();
        DACE::AlgebraicVector<double> wdt = 2.0*( InitSet - InitSet.cons()).eval(0.5*w);
        for ( int i=0; i < wdt.size(); ++i) {
            wdt[i] = std::fabs(wdt[i]);
        }

        for ( int k=0; k < wdt.size(); ++k) {
            if (wdt[k] != 0.0) ptUnit[k] = 2.0*(pt[k] - cnt[k])/wdt[k];
            else ptUnit[k] = 0.0;
        }
        std::cout<<ptUnit;
        SplittingHistory empty;
        if (!empty.contain(ptUnit) ) throw std::runtime_error ("error in 'Manifold::pointEvaluationManifold': The selected point seems to be outside the initial Domain");

        const unsigned int size = this -> size();
        for ( unsigned int i = 0; i < size; ++i ) {

            if ( (*this).at(i).history.contain(ptUnit) ) {

                DACE::AlgebraicVector<double> cP((*this).at(i).history.center() );
                DACE::AlgebraicVector<double> wP((*this).at(i).history.width() );
                DACE::AlgebraicVector<double> ptPatch = 2.0*(ptUnit - cP)/wP;
                return this -> at(i).eval(ptPatch);
            }
            /*In case that the manifold is been modified, i.e delate some Patch,
            and the point belongs to a delated Patch, the member function riturn
            a NaN vector ! ATTENTION*/
            if ( i == size-1 ) {
                DACE::AlgebraicVector<double> Null(this -> at(0).size(), NAN );
                return Null;
            }
        }
    }
}

void Manifold::set_integrator_ptr(integrator* integrator)
{
    // Setting problem object
    if (this->integrator_ != nullptr)
    {
        // If problem is not nullptr
        std::fprintf(stdout, "There already exist one problem class set in: '%p'. It will"
                             " be replaced.", this->integrator_);
    }

    // Replace problem pointer
    this->integrator_ = integrator;

    // Info
    std::fprintf(stdout, "Integrator object pointer ('%p') successfully set in Manifold ('%p').\n",
                 this->integrator_, this);

}

integrator *Manifold::get_integrator_ptr()
{
    // Safety check it is not empty
    if (this->integrator_ == nullptr)
    {
        // Info
        std::fprintf(stdout, "Problem class to be returned is nullptr. Errors may happen later...");
    }

    return this->integrator_;
}
