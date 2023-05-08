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
    integrator->set_errToll(this->errToll_);
    integrator->set_nSplitMax(this->nSplitMax_);

    // Replace problem pointer
    this->current_->set_integrator_ptr(integrator);

    // Info
    std::fprintf(stdout, "SuperManifold successfully built in: '%p'\n", this);
    std::fprintf(stdout, "\t-> Previous Manifold in: '%p'\n", this->previous_);
    std::fprintf(stdout, "\t-> Current Manifold in: '%p'\n", this->current_);

}

void SuperManifold::set_initial_manifold(Manifold *manifold)
{
    // Set initial manifold
    this->current_ = manifold;
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

    // Current passes to be previous
    this->previous_ = this->current_;

    // Split domain: get current domain
    this->current_ = this->current_->getSplitDomain(this->errToll_, this->nSplitMax_);
}