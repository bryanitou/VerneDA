/**
 * Super manifold class that will be dealing with manifold blocks.
 */

#include "SuperManifold.h"

void SuperManifold::set_integrator_ptr(integrator* integrator)
{
    // Safety checks
    if (integrator == nullptr)
    {
        // If problem is not nullptr
        std::fprintf(stderr, "Passed integrator pointer is nullptr!\n");

        // Exit program
        std::exit(-1);
    }

    // Creating new current manifold from integrator
    this->current_ = new Manifold(integrator->get_scv());

    // Set there the error tolerances and the maximum number of splits
    if (this->algorithm_ == ALGORITHM::ADS)
    {
        integrator->set_errToll(this->errToll_);
    }
    else if (this->algorithm_ == ALGORITHM::LOADS)
    {
        integrator->set_nli_threshold(this->nli_threshold_);
    }
    else
    {
        // TODO: Show some error messages here..
        std::fprintf(stderr, "Error to be written.");
    }

    integrator->set_nSplitMax(this->nSplitMax_);

    // Replace problem pointer
    this->current_->set_integrator_ptr(integrator);

    // Info
    std::fprintf(stdout, "SuperManifold successfully built in: '%p'\n", this);
    std::fprintf(stdout, "\t-> Previous Manifold in: '%p'\n", this->previous_);
    std::fprintf(stdout, "\t-> Current Manifold in: '%p'\n", this->current_);

}

void SuperManifold::split_domain()
{
    // Safety check that current manifold is available
    if (this->current_ == nullptr)
    {
        // Throw FTL
        std::fprintf(stderr, "Current manifold is nullptr! Must be set prior to split it!\n");

        // Exit program
        std::exit(-1);
    }

    // Current passes to be previous in a new pointer
    this->previous_ =  new Manifold(*this->current_);

    // Split domain: get current domain
    if (this->algorithm_ == ALGORITHM::ADS)
    {
        this->current_ = this->current_->getSplitDomain(this->errToll_, this->nSplitMax_);
    }
    else if (this->algorithm_ == ALGORITHM::LOADS)
    {
        this->current_ = this->current_->getSplitDomain(this->algorithm_, this->nSplitMax_);
    }
    else
    {
        // TODO: Show some error messages here..
        std::fprintf(stderr, "Error to be written.");
    }

}
