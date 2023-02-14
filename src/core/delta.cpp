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

void delta::compute_deltas(DISTRIBUTION distribution, int n, STATE state)
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
            this->generate_gaussian_deltas(n, state);
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

void delta::generate_gaussian_deltas(int n, STATE state)
{
    // Stack results here: no need to initialize!
    std::vector<std::shared_ptr<scv>> deltas;

    // Check what we have to change
    auto attempt_pos = tools::enums::STATE2POSITION(state);
    auto attempt_vel = tools::enums::STATE2VELOCITY(state);
    std::string var2change = attempt_pos != POSITION::NA ? "POSITION" : "VELOCITY";

    // Compute deltas: by hard set values
    // n: experiments
    // pos: position (x, y, z) that will be modified
    // Call to random engine generator
    std::default_random_engine generator;
    std::normal_distribution<double> distribution(var2change == "POSITION" ?  100.0 : 10,
                                                  var2change == "POSITION" ? 10 : 1); // 100 m centered and 10 m stddev

    // Reserve memory for CPU efficiency
    deltas.reserve(n);

    for (int i=0; i<n; ++i)
    {
        // Generate number from GAUSSIAN distribution
        double number = distribution(generator);

        // TODO: Remove when clear
        // Create Delta vector for DEBUG
        // DACE::AlgebraicVector<DACE::DA> Deltax0(6);
        // Deltax0[0] = 1.0;
        // std::cout << Deltax0.toString() << std::endl;


        // Generate the scv from the base csv
        auto scv_delta = std::make_shared<scv>(
                0.0, 0.0, 0.0, 0.0, 0.0, 0.0, false);

        // Change its value from the number given form the normal function
        // Make the change in the adequate place: px, py, pz, vx, vy, vz...
        // TODO: Revise this step:
        // auto delta = scv_base_->get_state_value_copy(pos) + number;
        // auto delta = number;

        // Set the delta in the given place
        // if (var2change == "POSITION")
        // {
        //     scv_delta->set_state_value(number, VELOCITY::X);
        //     scv_delta->set_state_value(number, VELOCITY::Y);
        //     scv_delta->set_state_value(number, VELOCITY::Z);
        // }
        // else
        // {
        //     scv_delta->set_state_value(number, VELOCITY::X);
        //     scv_delta->set_state_value(number, VELOCITY::Y);
        //     scv_delta->set_state_value(number, VELOCITY::Z);
        // }

        // TODO: Discuss this logic, leave this demonstration for the while
        scv_delta->set_state_value(number, POSITION::X);
        scv_delta->set_state_value(number, POSITION::Y);
        scv_delta->set_state_value(number, POSITION::Z);
        scv_delta->set_state_value(number, VELOCITY::X);
        scv_delta->set_state_value(number, VELOCITY::Y);
        scv_delta->set_state_value(number, VELOCITY::Z);

        // Append scv in the list
        deltas.emplace_back(scv_delta);
    }

    // Make shared and save
    this->scv_deltas_ = std::make_shared<std::vector<std::shared_ptr<scv>>>(deltas);

    // Print first one
    // std::cout << this->scv_deltas_->front()->get_state_vector_copy().front().toString() << std::endl;
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
        taylor_list.emplace_back(this->base_poly_->eval(scv_delta->get_state_vector_copy()));
    }

    // Make it ptr
    this->deltas_poly_ = std::make_shared<std::vector<DACE::AlgebraicVector<DACE::DA>>>(taylor_list);
}
