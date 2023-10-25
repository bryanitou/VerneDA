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
        // Set error tolerances
        integrator->set_errToll(this->errToll_);

        // Set the NSPLIT max
        integrator->set_nSplitMax(this->nSplitMax_);
    }
    else if (this->algorithm_ == ALGORITHM::LOADS)
    {
        // Set the NLI threshold
        integrator->set_nli_threshold(this->nli_threshold_);

        // Set the NSPLIT max
        integrator->set_nSplitMax(this->nSplitMax_);
    }
    else if (this->algorithm_ == ALGORITHM::NONE)
    {
        // NONE algorithm will be used
        std::fprintf(stderr, "Super Manifold: No algorithm for splitting will be used!\n");
    }
    else
    {
        // TODO: Show some error messages here..
        std::fprintf(stderr, "Super Manifold: Error to be written.\n");
    }

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
    if (this->algorithm_ != ALGORITHM::NA)
    {
        // Integrate and/or split
        this->current_ = this->current_->getSplitDomain(this->algorithm_, this->nSplitMax_);
    }
    else
    {
        // No algorithm has been chosen...
        std::fprintf(stderr, "Error to be written. SuperManifold.cpp line 84.\n");
    }

}

Manifold *SuperManifold::get_manifold_ini() const
{
    // Safety checks
    if (!this->current_)
    {
        // Throw FTL
        std::fprintf(stderr, "Current manifold is nullptr! Cannot compute the initial split domain box!\n");

        // Exit program
        std::exit(-1);
    }

    return this->current_->get_initial_split_domain();
}


void SuperManifold::set_6dof_domain()
{
    if (this->current_)
    {
        // Set pointers
        this->att_6dof_fin = new Manifold(*this->current_);

        // Convert patches
        for (auto & p : *this->att_6dof_fin)
        {
            auto euler_p = quaternion::quaternion2euler_DACE(p[0], p[1], p[2], p[3]);

            p = {
                    euler_p[0],
                    euler_p[1],
                    euler_p[2],
                    p[4],
                    p[5],
                    p[6],
                 };
        }
    }
    if (this->previous_)
    {
        this->att_6dof_ini = new Manifold(*this->previous_);
        // Convert patches
        for (auto & p : *this->att_6dof_ini)
        {
            auto euler_p = quaternion::quaternion2euler_DACE(p[0], p[1], p[2], p[3]);

            p = {
                    euler_p[0],
                    euler_p[1],
                    euler_p[2],
                    p[4],
                    p[5],
                    p[6],
            };
        }
    }
}