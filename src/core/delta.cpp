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

void delta::compute_deltas(DISTRIBUTION distribution, int n, STATE state, bool attitude)
{
    // Safety check that the deltas are still not generated
    if (this->scv_deltas_ != nullptr)
    {
        // WARNING: deltas are gonna be replaced
        std::printf("WARNING: Deltas are going to be replaced! I cannot save values");
    }

    // Safety check that the constants are already set
    if (!this->constants_set)
    {
        // WARNING! Must set constants for the distribution competition before
        std::printf("WARNING: Deltas are not going to be computed. Need to provide pos/vel: mean and stddev. "
                    "It has not been set yet. Exiting program.");

        // Exit program
        std::exit(-1);
    }

    // If normal distribution chosen:
    switch (distribution)
    {
        case DISTRIBUTION::GAUSSIAN:
        {
            this->generate_gaussian_deltas(n, state, attitude);
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

void delta::generate_gaussian_deltas(int n, STATE state, bool attitude)
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
    // todo: the mean is the initial condition, see photo
    std::normal_distribution<double> distribution_pos(this->mean_pos_, this->stddev_pos_);
    std::normal_distribution<double> distribution_vel(this->mean_vel_, this->stddev_vel_);

    // Reserve memory for CPU efficiency
    deltas.reserve(n);

    for (int i=0; i<n; ++i)
    {
        // Generate number from GAUSSIAN distribution
        double random_pos = distribution_pos(generator);
        double random_vel = distribution_vel(generator);

        // TODO: Discuss this logic, leave this demonstration for the while
        std::vector<DACE::DA> new_delta;
        if (attitude)
        {
            // TODO: To check: KENT DISTRIBUTION OR THE LINK IN quaternions.cpp
            // Get the quaternion
            auto nq = quaternion::euler2quaternion(random_pos - mean_pos_,
                                                   random_pos - mean_pos_,
                                                   random_pos - mean_pos_);

            // Safety check for norm
            double nq_norm = quaternion::getnorm(nq);
            bool is1norm = nq_norm == 1.0;

            if (!is1norm)
            {
                std::string err_msg = tools::string::print2string(
                        "This quaternion = ('%.2f', ''%.2f', '%.2f', ''%.2f') is not norm 1! Actual norm: '%.24f'",
                        nq[0], nq[1], nq[2], nq[3], nq_norm);
                std::cerr << err_msg << std::endl;
                std::exit(-1);
            }
            new_delta = {
                    nq[0], nq[1], nq[2], nq[3],
                    random_vel - mean_vel_,
                    random_vel - mean_vel_,
                    random_vel - mean_vel_};
        }
        else
        {
            new_delta = {
                    random_pos - mean_pos_,
                    random_pos - mean_pos_,
                    random_pos - mean_pos_,
                    random_vel - mean_vel_,
                    random_vel - mean_vel_,
                    random_vel - mean_vel_};
        }


        // Generate the scv from the base csv
        auto scv_delta = std::make_shared<scv>(new_delta);

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

    // Reserve space for optimal memory management
    taylor_list.reserve(this->scv_deltas_->size());

    // Evaluate each delta
    for (const auto& scv_delta : *scv_deltas_)
    {
        // Evaluate and save
        taylor_list.emplace_back(this->base_poly_->eval(scv_delta->get_state_vector_copy()));

        // TODO: DEBUG LINE
        std::cout << this->base_poly_->eval(scv_delta->get_state_vector_copy()).cons().extract(0, 3).vnorm() << std::endl;
    }

    // Make it ptr
    this->deltas_poly_ = std::make_shared<std::vector<DACE::AlgebraicVector<DACE::DA>>>(taylor_list);
}

void delta::set_constants(double mean_pos, double stddev_pos, double mean_vel, double stddev_vel)
{
    // Set constants
    this->mean_pos_ = mean_pos;
    this->stddev_pos_ = stddev_pos;
    this->mean_vel_ = mean_vel;
    this->stddev_vel_ = stddev_vel;

    // Notice
    this->constants_set = true;

}
