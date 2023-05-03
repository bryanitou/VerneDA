/**
 * Super manifold class that will be dealing with manifold blocks.
 */

#include "SuperManifold.h"

void SuperManifold::set_problem_object(problems* probl)
{
    // Setting problem object
    if (this->probl_ != nullptr)
    {
        // If problem is not nullptr
        std::fprintf(stdout, "There already exist one problem class set in: '%p'. It will"
                             " be replaced.", this->probl_);
    }

    // Replace problem pointer
    this->probl_ = probl;

    // Info
    std::fprintf(stdout, "Problem object pointer ('%p') successfully set in Manifold ('%p').",
                 this->probl_, this);

}

problems *SuperManifold::get_problem_object()
{
    // Safety check it is not empty
    if (this->probl_ == nullptr)
    {
        // Info
        std::fprintf(stdout, "Problem class to be returned is nullptr. Errors may happen later...");
    }

    return this->probl_;
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
        std::fprintf(stderr, "Current manifold is nullptr! Must be set prior to split it!");

        // Exit program
        std::exit(-1);
    }


    // Current passes to be previous
    this->previous_ = this->current_;

    // Update problem
    this->current_->set_problem_object(this->probl_);

    // Split domain: get current domain
    this->current_ = this->current_->getSplitDomain(this->errToll_, this->nSplitMax_, t);

}