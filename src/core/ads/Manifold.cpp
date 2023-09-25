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

Manifold::Manifold( const Manifold& m) : std::deque< Patch >(m)
{
    this->integrator_ = m.integrator_;
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

Manifold* Manifold::getSplitDomain(ALGORITHM algorithm, int nSplitMax, bool domain_evolution)
{
    /* (Low Order?) Automatic Domain Splitting Algorithm */
    auto results = new Manifold();

    // Re-set integrator
    results->integrator_ = this->integrator_;

    // Iterator
    int i = 0;
    int split_count = 1;

    /*execute steps of Automatic Domain Splitting, calling the function to estimate the error and to split the Patch*/
    // While runs until the vector gets emptied >> (std::deque< Patch >)
    while (!this->empty())
    {
        // Print status
        this->print_status();

        // Remove this when problem solved
        if (this->size() == 126)
        {
            bool a = true;
        }

        // Debugging information
        if (domain_evolution)
        {
            // Make a copy and store its pointer
            results->ini_domain_record->push_back(new Manifold(*this));
            results->fin_domain_record->push_back(new Manifold(*results));
        }

        // Creates new patch from the first position in this Manifold
        auto p = this->front();

        // Removes the one in front
        this->pop_front();

        // TODO: DEBUG CODE
        if (p.get_history_count() == 6 || i == 121)
        {
            bool a = true;
        }

        // Set time for the integrator
        this->integrator_->t_ = p.t_;

        // Set the beta from the patch to the integrator
        if (i != 0)
        {
            this->integrator_->betas_ = p.betas;
        }


        // Get the new state
        auto scv = this->integrator_->integrate(p, p.id_);

        // Builds patch from the resulting scv
        Patch f(scv, p.get_history_int(), p.get_times_doubles(), p.get_nlis_doubles(), algorithm, this->integrator_->t_, p.nli, p.t_split_);

        if (f.get_history_count() == nSplitMax || this->integrator_->end_) // TODO: What about this case: (*max_error == 0.0) See old function
        {
            // Check the maximum function error and the total number of split for the Patch
            results->push_back(f);
        }
        else
        {
            // Get direction of the split
            auto dir = this->integrator_->get_splitting_pos() + 1;

            // Split the patch
            auto s = f.split(dir);

            // Add new patches
            for (auto & p_new : s)
            {
                // To cunt this we have to do several trials
                p_new.id_ = split_count;
                p_new.nli = this->integrator_->nli_current_;
                p_new.t_split_ = this->integrator_->t_;
                p_new.betas = this->integrator_->betas_;
                p_new.betas[dir - 1] /= 3;

                // TODO: push back of this object... copies are lost? Analyze what's happening in memory
                this->push_back(p_new);

                // Increase split count
                split_count++;
            }

            // TODO: DEBUG CODE
            if (s[0].get_history_count() == 6)
            {
                bool a = true;
            }

            // split_count += 3;
            // std::ofstream file2write;
            // file2write.open("/home/bryan/CLionProjects/ISAE/research_project/VerneDA/out/example/loads/n_split_vs_time.csv", std::ios_base::app);
            // file2write  << std::to_string(this->integrator_->t_ / (M_PI * 2)) << "," << std::to_string(split_count) << std::endl;
            // // // Close the stream
            // file2write.close();
        }

        i++;
    }

    return results;
}

/*
Manifold* Manifold::getSplitDomain(const std::vector<double>& errToll, const int nSplitMax, int posOverride)
{
    //  Member function elaborating the ADS of initial Manifold (initial Domain)
    //  !>> input: (*func) is the expansion function whose error is estimate
    //  std::vector<double> errToll is the threshold for the estimation error for each component of Patch
    //  double nSplitMax is the threshold for the maximum number of split for each box
    //  !<< return Manifold containing the generating Patch

    // Automatic Domain Splitting
    auto results = new Manifold();

    // Re-set integrator
    results->integrator_ = this->integrator_;

    // execute steps of Automatic Domain Splitting, calling the function to estimate the error and to split the Patch
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
        Patch f(scv, p.history, this->integrator_->t_, this->integrator_->nli_current_);

        // Check for tolerance
        // TODO: This check needs to be done only once
        if (errToll.size() != f.size())
        {
            // Build err msg
            auto err_msg = tools::string::print2string("Error in Manifold::getSplitDomain: The Tolerance vector must have the same "
                                                       "size of Patchs. Size patch: '%d'. Error tolerance size: '%d'", f.size(), errToll.size());
            // Throw err
            throw std::runtime_error (err_msg);
        }

        // Estimate the error for each patch in the deque
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
            auto s = p.split(dir);

            // Add new manifolds
            this->push_back(s[0]); // Left patch
            this->push_back(s[1]); // Right patch
        }
    }

    return results;
}
*/
/**
Manifold Manifold::getSplitDomain(DACE::AlgebraicVector<DACE::DA> (*func)(DACE::AlgebraicVector<DACE::DA> ), const double errToll, const int nSplitMax, int posOverride) {
  // Member function elaborating the ADS of initial Manifold (initial Domain)
  !>> input: (*func) is the expansion function whose error is estimate
             double errToll is the threshold for the estimation error
             double nSplitMax is the threshold for the maximun number of split for each box
   !<< return Manifold containing the generating Patch

    // automatic Domain splitting

    Manifold results;

    // execute steps of automatic domain splitting, calling the function to eximate the error and to split the Patch
    while ( !this -> empty() ) {

        Patch p = this -> front();
        this -> pop_front();

        Patch f(func(p), p.get_history_int());

        // Estimate the error for each patch in the deque
        auto error = f.getTruncationErrors();
        std::vector<double>::iterator max_error = std::max_element(error.begin(), error.end());   // find maximum truncation error
        if ( posOverride != 0) max_error = (error.begin() + posOverride);

        if ( *max_error < errToll || p.get_history_int().count() == nSplitMax) { //check the maximum function error and the total number of split for the Patch
            results.push_back(f);
        }
        else {
            const unsigned int pos = std::distance(error.begin(), max_error);  // function component of maximum error
            const unsigned int dir = f.getSplittingDirection(pos);
            auto s = p.split(dir);
            this -> push_back(s[0]);
            this -> push_back(s[1]);
        }

    }

    return results;
}

Manifold Manifold::getSplitDomain(DACE::AlgebraicVector<DACE::DA> (*func)(DACE::AlgebraicVector<DACE::DA>, double ), const double errtol, const int nSplitMax, const double mu, int posOverride) {
  // Member function elaborating the ADS of initial Manifold (initial Domain)
  !>> input: (*func) is the expansion function whose error is estimate
             double errtol is the threshold for the estimation error
             double nSplitMax is the threshold for the maximun number of split for each box
   !<< return Manifold containing the generating Patch

    // automatic Domain splitting

    Manifold results;

    // execute steps of automatic domain splitting, calling the function to eximate the error and to split the Patch
    while ( !this -> empty() ) {

        Patch p = this -> front();
        this -> pop_front();

        Patch f(func(p, mu), p.history);

        // Estimate the error for each patch in the deque
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
            this -> push_back(s[0]);
            this -> push_back(s[1]);
        }

    }

    return results;
}

Manifold Manifold::getSplitDomain(DACE::AlgebraicVector<DACE::DA> (*func)(DACE::AlgebraicVector<DACE::DA>, double), const std::vector<double> errToll, const int nSplitMax, const double mu, int posOverride) {
    /* Member function elaborating the ADS of initial Manifold (initial Domain)
     !>> input: (*func) is the expansion function whose error is estimate
     std::vector<double> errToll is the threshold for the estimation error for aech component of Patch
     double nSplitMax is the threshold for the maximun number of split for each box
     !<< return Manifold containing the generating Patch
    
    //automatic Domain splitting
    
    Manifold results;

    while ( !this -> empty() ) {
        
        Patch p = this -> front();
        this -> pop_front();
        
        Patch f(func(p, mu), p.history);
        if ( errToll.size() != f.size() ) throw std::runtime_error ("Error in Manifold::getSplitDomain: The Tollerance vector must have the same size of Patchs ");

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
            this -> push_back(s[0]);
            this -> push_back(s[1]);
        }
        
    }
    
    return results;
}
Manifold Manifold::getSplitDomain(DACE::AlgebraicVector<DACE::DA> (*func)(DACE::AlgebraicVector<DACE::DA>, Observable*, double), const double errToll, const int nSplitMax, Observable* param, const double mu, int posOverride) {
  // Member function elaborating the ADS of initial Manifold (initial Domain)
  !>> input: (*func) is the expansion function whose error is estimate
             double errToll is the threshold for the estimation error
             double nSplitMax is the threshold for the maximun number of split for each box
   !<< return Manifold containing the generating Patch

    automatic Domain splitting

    Manifold results;

    // execute steps of automatic domain splitting, calling the function to eximate the error and to split the Patch
    while ( !this -> empty() ) {

        Patch p = this -> front();
        this -> pop_front();

        Patch f(func(p, param, mu), p.history);

        // Estimate the error for each patch in the deque
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
            this -> push_back(s[0]);
            this -> push_back(s[1]);
        }

    }

    return results;
}

Manifold Manifold::getSplitDomain(DACE::AlgebraicVector<DACE::DA> (*func)(DACE::AlgebraicVector<DACE::DA>, Observable*, double), const std::vector<double> errToll, const int nSplitMax, Observable* param, const double mu, int posOverride) {
  // Member function elaborating the ADS of initial Manifold (initial Domain)
  !>> input: (*func) is the expansion function whose error is estimate
             std::vector<double> errToll is the threshold for the estimation error for aech component of Patch
             double nSplitMax is the threshold for the maximun number of split for each box
   !<< return Manifold containing the generating Patch

    // automatic Domain splitting

    Manifold results;

    // execute steps of automatic domain splitting, calling the function to eximate the error and to split the Patch
    while ( !this -> empty() ) {

        Patch p = this -> front();
        this -> pop_front();

        Patch f(func(p, param, mu), p.history);
        if ( errToll.size() != f.size() ) throw std::runtime_error ("Error in Manifold::getSplitDomain: The Tollerance vector must have the same size of Patchs ");

        Estimate the error for each patch in the deque
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
            this -> push_back(s[0]);
            this -> push_back(s[1]);
        }

    }

    return results;
}

**/
void Manifold::print_status()
{
    std::string msg2write = tools::string::print2string("TRACE: Manifold size: %10d", this->size());

    // Print msg
    std::fprintf(stdout, "%s\n", msg2write.c_str());
}


DACE::AlgebraicVector<double> Manifold::pointEvaluationManifold(const DACE::AlgebraicVector<DACE::DA>& InitSet, DACE::AlgebraicVector<double> pt, const int flag)
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

    /*
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

            // In case that the manifold is been modified, i.e delete some Patch,
            and the point belongs to a deleted Patch, the member function return
            a NaN vector ! ATTENTION
            if ( i == size-1 )
            {
                DACE::AlgebraicVector<double> Null(this -> at(0).size(), NAN );
                return Null;
            }
        }
    }*/

    if ( flag == 1 )
    {
        // Get amount of variables
        const unsigned int comp = DACE::DA::getMaxVariables();

        // Create vectors:
        // ptUnit: dimension ['comp']
        // cnt: dimension ['comp']
        DACE::AlgebraicVector<double> ptUnit(comp);

        // Create vector w: width
        // dimension ['comp'] and value 2, from -1 to 1
        DACE::AlgebraicVector<double> w(comp, 2.0);

        // Normalize?
        DACE::AlgebraicVector<double> wdt = 2.0*(InitSet - InitSet.cons()).eval(0.5*w);
        // DACE::AlgebraicVector<double> wdt = 2.0*(InitSet).eval(0.5*w);

        // Make it absolute values
        for (double & i : wdt)
        {
            i = std::fabs(i);
        }

        // Iterate through all the variables
        for ( int k=0; k < wdt.size(); ++k)
        {
            // If the width is different than zero...
            if (wdt[k] != 0.0)
            {
                //
                // ptUnit[k] = 2.0 * (pt[k] - cnt[k]) / wdt[k];
                ptUnit[k] = 2.0 * (pt[k]) / wdt[k];
            }
            else
            {
                ptUnit[k] = 0.0;
            }
        }

        // Get the size of this manifold (amount of patches stored)
        const unsigned int size = this->size();

        // Print normalized point
        // std::cout << ptUnit;

        // Create splitting history object: 'empty'
        SplittingHistory empty;

        // Check if the normalized point is within the limit box
        if (!empty.contain(ptUnit, this->integrator_->get_algorithm()))
        {
            // If here, it means that point lies outside the initial domain, should check for the nearest patch
            double distance = INFINITY;
            unsigned int patch_idx = INFINITY;

            // Iterate through the size of this manifold
            for ( unsigned int i = 0; i < size; ++i )
            {
                // If so, get the center
                DACE::AlgebraicVector<double> cP((*this).at(i).get_center());

                // Get the width
                DACE::AlgebraicVector<double> wP((*this).at(i).get_width());

                // Get the distance
                double dist_patch_i = (cP - ptUnit).vnorm();

                // If that distance is inferior that the minimum...
                if (dist_patch_i < distance)
                {
                    distance = dist_patch_i;
                    patch_idx = i;
                }
            }

            // If so, get the center
            DACE::AlgebraicVector<double> cP((*this).at(patch_idx).get_center());

            // Get the width
            DACE::AlgebraicVector<double> wP((*this).at(patch_idx).get_width());

            // Evaluate point in the nearest patch
            DACE::AlgebraicVector<double> ptPatch = 2.0 * (ptUnit - cP) / wP;

            // Evaluate expression in the
            auto result = this->at(patch_idx).eval(ptPatch);

            // Get new string
            auto vector2write = tools::vector::num2string(ptUnit, ", ");
            auto vector2write_patch = tools::vector::num2string(cP, ", ");

            // Info
            std::fprintf(stdout, "Sample vector violated limit box: '%s'. "
                                 "Evaluated to nearest patch (id: '%d') with centers: '%s'\n",
                                 vector2write.c_str(), patch_idx, vector2write_patch.c_str());

            // Exit function returning result
            return result;
        }

        // Iterate through the size of this manifold
        for ( unsigned int i = 0; i < size; ++i )
        {
            // Is this polynomial within the limit box?
            auto inside_limits = (*this).at(i).history_contains(ptUnit);

            // Is within limits?
            if (inside_limits)
            {
                // If so, get the center
                DACE::AlgebraicVector<double> cP((*this).at(i).get_center());

                // Get the width
                DACE::AlgebraicVector<double> wP((*this).at(i).get_width());

                // Center the normalized 'ptUnit' vector to the center of the patch, to be evaluated
                // later
                DACE::AlgebraicVector<double> ptPatch = 2.0 * (ptUnit - cP) / wP;

                // Evaluate expression in the
                auto result = this->at(i).eval(ptPatch);

                // Exit function returning result
                return result;
            }


            /*
             * In case that the manifold is been modified, i.e delete some Patch,
             * and the point belongs to a delated Patch, the member function return
             * a NaN vector !
             * ATTENTION
             */
            if ( i == size - 1 )
            {
                // Create Null vector
                DACE::AlgebraicVector<double> Null(this -> at(0).size(), NAN);

                // Return Null vector
                return Null;
            }
        }
    }

    // TODO: Add fallback
    return {};
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
    std::fprintf(stdout, "Manifold: Integrator object pointer ('%p') successfully set in Manifold ('%p').\n",
                 this->integrator_, this);

}

std::vector<DACE::AlgebraicVector<double>> Manifold::centerPointEvaluationManifold()
{
    // Result vector to be returned
    std::vector<DACE::AlgebraicVector<double>> result{};

    // Get the size of this manifold (amount of patches stored)
    const unsigned int size_manifold = this->size();

    // Reserve memory for optimal memory management
    result.reserve(size_manifold);

    // Get number of variables
    DACE::AlgebraicVector<double> zeroed(DACE::DA::getMaxVariables(), 0);

    // Iterate through the size of this manifold
    for ( unsigned int i = 0; i < size_manifold; ++i )
    {
        // Get the translated result fo this patch
        auto center_i_transposed = this->at(i).eval(zeroed);

        if (this->integrator_->get_problem_ptr()->get_type() == PROBLEM::FREE_TORQUE_MOTION)
        {
            // Get the constants
            auto quaternion = center_i_transposed.cons().extract(0, 3);

            // Convert to Euler
            auto euler_angles = quaternion::quaternion2euler(quaternion[0],
                                                             quaternion[1],
                                                             quaternion[2],
                                                             quaternion[3]);

            // Replace
            result.push_back({euler_angles[0], euler_angles[1], euler_angles[2],
                              center_i_transposed[4], center_i_transposed[5], center_i_transposed[6]});
        }
        else
        {
            // Push back result
            result.push_back(center_i_transposed);
        }
    }

    // Exit function
    return result;
}

std::vector<std::vector<DACE::AlgebraicVector<double>>> Manifold::wallsPointEvaluationManifold()
{
    // Result vector to be returned
    std::vector<std::vector<DACE::AlgebraicVector<double>>> result{};

    // Get the size of this manifold (amount of patches stored)
    const unsigned int size_manifold = this->size();

    // Reserve memory for optimal memory management
    result.reserve(size_manifold);

    // Get number of variables
    const unsigned int n_var = DACE::DA::getMaxVariables();

    // We always will be only projecting the X-Y variable
    std::vector<int> sweep (n_var, 0); sweep[0] = 1; sweep[1] = 1;

    // We want to draw the path in a coherent order: X right, Y down, X left, Y up
    std::vector<bool> path = {true, false, false, true};

    // Get all the points to be evaluated
    auto wall_points2eval = tools::math::hypercubeEdges((int) n_var, 2, sweep, path);

    // Get size of wall
    unsigned int n_size_wall = wall_points2eval.size();

    // Iterate through the size of this manifold
    for ( unsigned int i = 0; i < size_manifold; ++i )
    {
        // Patch walls
        std::vector<DACE::AlgebraicVector<double>> image_patch_walls;

        // Reserve memory: 4 walls and res
        image_patch_walls.reserve(n_size_wall);

        for (int k = 0; k < n_size_wall; k++)
        {
            // Get the translated result fo this patch
            auto image_wall_point = this->at(i).eval(wall_points2eval[k]);

            // Convert to euler angles if required
            if (this->integrator_->get_problem_ptr()->get_type() == PROBLEM::FREE_TORQUE_MOTION)
            {
                // Convert to Euler
                auto euler_angles = quaternion::quaternion2euler(
                        image_wall_point[0],
                        image_wall_point[1],
                        image_wall_point[2],
                        image_wall_point[3]
                        );

                // Replace
                image_wall_point = {euler_angles[0], euler_angles[1], euler_angles[2],
                                  image_wall_point[4], image_wall_point[5], image_wall_point[6]};
            }

            // Push back the first point: corner
            image_patch_walls.emplace_back(image_wall_point);
        }

        // Push back this patch wall points
        result.push_back(image_patch_walls);
    }

    // Exit function
    return result;
}

std::vector<std::vector<DACE::AlgebraicVector<double>>> Manifold::wallsPointEvaluationManifold_useless()
{
    // Result vector to be returned
    std::vector<std::vector<DACE::AlgebraicVector<double>>> result{};

    // Get the size of this manifold (amount of patches stored)
    const unsigned int size_manifold = this->size();

    // Reserve memory for optimal memory management
    result.reserve(size_manifold);

    // Get number of variables
    const unsigned int n_var = DACE::DA::getMaxVariables();

    // Auxiliary variables fot the internal loop
    DACE::AlgebraicVector<double> point_wall(n_var, 0);

    // Fix resolution
    int res = 10;

    // Fix dimension
    int dim = 2;

    // Fix number of walls per dimension
    int n_wall = static_cast<int>(std::pow(dim, 2));

    // Compuote number of points
    int n_points = n_wall*res;

    // Iterate through the size of this manifold
    for ( unsigned int i = 0; i < size_manifold; ++i )
    {
        // In 2D, we will have 4 walls, in 3D 6 walls... and so forth
        // It will only be done in 2D, for validation purposes for the while
        // Parameters for this wall
        // Get the upper and lower wall

        // Get the width
        double width_x = 2;
        double width_y = 2;
        double half_width_x = width_x / 2;
        double half_width_y = width_y / 2;

        // Compute the step
        double step_x = width_x / res;
        double step_y = width_y / res;

        // Now, we will have to bring them to: x: bottom and top, y: left and right
        // Start at the bottom left
        point_wall[0] = half_width_x;
        point_wall[1] = half_width_y;

        // Patch walls
        std::vector<DACE::AlgebraicVector<double>> image_patch_walls;

        // Reserve memory: 4 walls and res
        image_patch_walls.reserve(n_points + 1);

        for (int k = 0; k < n_points + 1; k++)
        {
            // Get the translated result fo this patch
            auto image_wall_point = this->at(i).eval(point_wall);

            // Convert to euler angles if required
            if (this->integrator_->get_problem_ptr()->get_type() == PROBLEM::FREE_TORQUE_MOTION)
            {
                // Get the constants
                auto quaternion = image_wall_point.cons().extract(0, 3);

                // Convert to Euler
                auto euler_angles = quaternion::quaternion2euler(quaternion[0],
                                                                 quaternion[1],
                                                                 quaternion[2],
                                                                 quaternion[3]);

                // Replace
                image_wall_point = {euler_angles[0], euler_angles[1], euler_angles[2],
                                  image_wall_point[4], image_wall_point[5], image_wall_point[6]};
            }

            // Push back the first point: corner
            image_patch_walls.push_back(image_wall_point);

            // Check positions
            bool d = std::fabs(point_wall[1] - (-half_width_y)) < 0.001;
            bool u = std::fabs(point_wall[1] - half_width_y) < 0.001;
            bool l = std::fabs(point_wall[0] - (-half_width_x)) < 0.001;
            bool r = std::fabs(point_wall[0] - half_width_x) < 0.001;

            // Build next point to be evaluated
            if (u && !r)
            {
                // Go right
                point_wall[0] += step_x;
            }
            else if (r && !d)
            {
                // Go down
                point_wall[1] -= step_y;
            }
            else if (d && !l)
            {
                // Go left
                point_wall[0] -= step_x;
            }
            else
            {
                // Go up
                point_wall[1] += step_y;
            }
        }

        // Push back this patch wall points
        result.push_back(image_patch_walls);
    }

    // Exit function
    return result;
}


Manifold* Manifold::get_initial_split_domain()
{
    // Create manifold to return TODO: What the fuck, why it is copied from this class? After it is cleared...
    auto splitbox = new Manifold(*this);
    splitbox->clear();

    // TODO: Reserve, since we know the amount of patches
    // splitbox->shrink_to_fit();

    // Replay history for every path
    for (auto & p : *this)
    {
        // Get the new patch
        auto new_p = p.replay();

        // Push back in the box manifold
        splitbox->push_back(new_p);
    }

    return splitbox;
}