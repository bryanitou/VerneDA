/**
 * Deltas will be computed in this namespace
 */

#include "delta.h"

delta::delta(scv &scv_base, DACE::AlgebraicVector<DACE::DA> &poly)
{
    // Call main allocator
    this->allocate_scv_base(scv_base, poly);
}

void delta::allocate_scv_base(scv& scv_base, DACE::AlgebraicVector<DACE::DA>& taylor_polinomial)
{
    // Allocate scv
    this->scv_base_ = std::make_shared<scv>(scv_base);

    // Allocate polynomial to evaluate
    this->base_poly_ = std::make_shared<DACE::AlgebraicVector<DACE::DA>>(taylor_polinomial);

}

void delta::compute_deltas(DISTRIBUTION distribution, int n, POSITION pos)
{
    // Safety check that the deltas are still not generated
    if (this->scv_deltas_ != nullptr)
    {
        // WARNING: deltas are gonna be replaced
        std::printf("WARNING: Deltas are going to be replaced! I cannot save values");
    }

    // If normal distribution chosen:
    switch (distribution)
    {
        case DISTRIBUTION::GAUSSIAN:
        {
            this->generate_gaussian_deltas(n, pos);
            break;
        }
        default:
        {
            // Throw FATAL
            std::printf("FATAL: Couldn't find enum '%s'",
                        tools::enums::DISTRIBUTION2str(distribution).c_str());
        }
    }

    // Once done, evaluate deltas with the polynomial which is already saved in memory
    this->evaluate_deltas();

    // TODO: Show info message
}

void delta::generate_gaussian_deltas(int n, POSITION pos)
{
    // Compute deltas: by hard set values
    // n: experiments
    // pos: position (x, y, z) that will be modified
    // Call to random engine generator
    std::default_random_engine generator;
    std::normal_distribution<double> distribution(1000, 100); // 1 km centered and 100 m standard deviation

    // Stack results here: no need to initialize!
    std::vector<std::shared_ptr<scv>> deltas;

    // Reserve memory for CPU efficiency
    deltas.reserve(n);

    for (int i=0; i<n; ++i)
    {
        // Generate number from GAUSSIAN distribution
        double number = distribution(generator);

        // Generate the scv from the base csv
        auto scv_delta = std::make_shared<scv>(*this->scv_base_);

        // Change its value from the number given form the normal function
        // Make the change in the adequate place: px, py, pz, vx, vy, vz...
        auto delta_in_place = scv_base_->get_state_value_copy(pos) + number;

        // Set the delta in the given place
        scv_delta->set_state_value(delta_in_place, pos);

        // Append scv in the list
        deltas.emplace_back(scv_delta);
    }

    // Make shared and save
    this->scv_deltas_ = std::make_shared<std::vector<std::shared_ptr<scv>>>(deltas);

    // TODO: Info message
}

void delta::evaluate_deltas()
{
    // Local auxiliary variables
    std::vector<DACE::AlgebraicVector<DACE::DA>> taylor_list;
    taylor_list.reserve(this->scv_deltas_->size());

    // Evaluate each delta
    for (const auto& scv_delta : *scv_deltas_)
    {
        // Evaluate and save
        taylor_list.emplace_back(this->base_poly_->eval(scv_delta->get_state_vector()));
    }

    // Make it ptr
    this->deltas_poly_ = std::make_shared<std::vector<DACE::AlgebraicVector<DACE::DA>>>(taylor_list);
}
