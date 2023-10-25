/**
 * Deltas will be computed in this namespace
 */

#include "delta.h"

#include <utility>

void delta::generate_deltas(DISTRIBUTION type, int n)
{
    // Place for the all safety checks before computing HERE BELOW:
    // Safety check that the deltas are still not generated
    if (this->scv_deltas_ != nullptr)
    {
        // WARNING: deltas are going to be replaced
        std::printf("WARNING: Deltas are going to be replaced! I cannot save values");
    }

    // Safety check that the constants are already set
    if (!this->stddevs_set_)
    {
        // WARNING! Must set constants for the distribution competition before
        std::printf("WARNING: Deltas are not going to be computed. Need to provide pos/vel: stddev. "
                    "It has not been set yet. Exiting program.");

        // Exit program
        std::exit(-1);
    }

    // Safety checks if it is attitude, since its more complex...
    if (this->attitude_)
    {
        // Call function checker TODO: this check is useless since mean_quaternion is never used
        this->attitude_safety_checks();
    }

    // If normal distribution chosen:
    switch (type)
    {
        // Check cases...
        case DISTRIBUTION::GAUSSIAN:
        {
            // Generate gaussian deltas machine
            this->generate_gaussian_deltas(n);
            break;
        }
        default:
        {
            // Throw FATAL
            std::fprintf(stderr,"FATAL: Couldn't find enum '%s'",
                         tools::enums::DISTRIBUTION2str(type).c_str());

            // Exit program
            std::exit(-1);
            break;
        }
    }
}

void delta::generate_gaussian_deltas(int n)
{
    // Auxiliary variable for quaternion
    std::vector<double> nq;

    // Stack results here: no need to initialize!
    std::vector<DACE::AlgebraicVector<double>> deltas;

    // Compute deltas: by hard set values
    // n: experiments
    // pos: position (x, y, z) that will be modified
    // Call to random engine generator
    std::default_random_engine generator;
    // todo: the mean is the initial condition, see photo
    std::vector< std::normal_distribution<double>> stddevs_distr;
    stddevs_distr.reserve(this->stddevs_.size());

    // Generate all distribution objects
    for (const auto & std : this->stddevs_)
    {
        // Get other distribution engines
        stddevs_distr.emplace_back(0.0, std);
    }


    // Reserve memory for CPU efficiency
    deltas.reserve(n + 1);

    for (int i=0; i<n; ++i)
    {
        // TODO: Discuss this logic, leave this demonstration for the while
        DACE::AlgebraicVector<double> new_delta;
        if (this->attitude_)
        {
            // TODO: To check: KENT DISTRIBUTION OR THE LINK IN quaternions.cpp
            // Clean quaternion before assigning
            nq.clear();

            // Get the quaternion
            switch (this->q_sampling_)
            {
                case QUATERNION_SAMPLING::EULER_GAUSSIAN:
                {
                    nq = quaternion::euler2quaternion(stddevs_distr[0](generator),
                                                      stddevs_distr[1](generator),
                                                      stddevs_distr[2](generator));
                    break;
                }
                case QUATERNION_SAMPLING::SEED_GAUSSIAN:
                {
                    nq = quaternion::q8_normal_01(1234);
                    break;
                }
                case QUATERNION_SAMPLING::OMPL_GAUSSIAN:
                {
                    nq = quaternion::euler2quaternion_fromGaussian(stddevs_distr[0](generator),
                                                                   stddevs_distr[1](generator),
                                                                   stddevs_distr[2](generator));
                    break;
                }
                default:
                {
                    std::printf("Cannot find how to sample the quaternion!");
                    break;
                }

            }

            // Generate new delta
            new_delta = {
                    nq[0] - 1, // I still don't like this...
                    nq[1] - 0, // I still don't like this...
                    nq[2] - 0, // I still don't like this...
                    nq[3] - 0, // I still don't like this...
                    stddevs_distr[3](generator),
                    stddevs_distr[4](generator),
                    stddevs_distr[5](generator)};
        }
        else
        {
            for (auto & k : stddevs_distr)
            {
                new_delta.push_back(k(generator));
            }
        }

        // Append scv in the list
        deltas.push_back(new_delta);
    }

    // Make shared and save
    this->scv_deltas_ = std::make_shared<std::vector<DACE::AlgebraicVector<double>>>(deltas);
}

void delta::evaluate_deltas()
{
    // Safety check
    if (!this->zeroed_inserted_)
    {
        std::fprintf(stderr, "FATAL: Cannot evaluate deltas!, Must insert the nominal first. Exiting program.");
        std::exit(-1);
    }
    // Local auxiliary variables
    std::vector<DACE::AlgebraicVector<double>> taylor_list;
    //DACE::AlgebraicVector<DACE::DA> single_sol;

    // Reserve space for optimal memory management
    taylor_list.reserve(this->scv_deltas_->size());

    // Evaluate each delta
    for (const auto& scv_delta : *this->scv_deltas_)
    {
        // Evaluate and save
        auto single_sol = this->sm_->get_manifold_fin()->pointEvaluationManifold(
                this->sm_->previous_->front(),
                scv_delta.cons(),
                1);
        if (single_sol.empty())
        {
            continue;
        }

        // Check the norm for DEBUG PURPOSES
        if (this->attitude_)
        {
            // Get line to write...
            auto line2write = tools::string::print2string("Norm after evaluation: '%.5f'",
                                                          single_sol.cons().extract(0, 3).vnorm());

            // Write line...
            std::fprintf(stdout, "DEBUG: %s\n", line2write.c_str());
        }


        // If it is attitude_, we should convert the quaternion to Euler angles
        if (this->quat2euler_)
        {
            // Get the constants
            auto quaternion = single_sol.cons().extract(0, 3);

            // Normalize quaternion
            // quaternion = quaternion / quaternion.vnorm();

            // Convert to Euler
            auto euler_angles = quaternion::quaternion2euler_NORMAL(quaternion[0],
                                                             quaternion[1],
                                                             quaternion[2],
                                                             quaternion[3]);

            if (scv_delta == scv_deltas_->back() && this->attitude_)
            {
                // Some debugging information
                auto scv_cons = scv_delta.cons();
                auto scv_cons_str = tools::vector::num2string<double>(scv_cons);
                double scv_q_norm = scv_delta.cons().extract(0, 3).vnorm();
                // INITIAL
                std::fprintf(stdout, "DEBUG: Initial state: %s, quaternion norm: '%.2f'.\n",
                             scv_cons_str.c_str(), scv_q_norm);

                // FINAL
                auto single_sol_cons = single_sol.cons();
                auto single_sol_cons_str = tools::vector::num2string<double>(single_sol_cons);
                double single_sol_q_norm = single_sol_cons.extract(0, 3).vnorm();
                std::fprintf(stdout, "DEBUG: Final state: %s, quaternion norm: '%.2f'.\n",
                             single_sol_cons_str.c_str(), single_sol_q_norm);

            }

            // Replace the constant valuesç
            // TODO: MAKE THIS MODULAR FROM THE MAIN
            single_sol = {euler_angles[0],
                          euler_angles[1],
                          euler_angles[2], single_sol[4], single_sol[5], single_sol[6]};

            // std::cout << single_sol << std::endl; DEBUG LINE
        }

        // Push back
        taylor_list.emplace_back(single_sol);
    }

    // Make it ptr
    this->eval_deltas_poly_ = std::make_shared<std::vector<DACE::AlgebraicVector<double>>>(taylor_list);
}

void delta::set_stddevs(const std::vector<double>& stddevs)
{
    // Set constants
    this->stddevs_ = stddevs;

    // Notice
    this->stddevs_set_ = true;

}

void delta::insert_nominal(int n)
{
    // Check if already exist
    if (!this->scv_deltas_)
    {
        // Throw error
        std::fprintf(stderr, "FATAL: Deltas have not been generated! Generate them before inserting the nominal SCV.");

        // Exit program
        std::exit(-1);
    }

    // The nominal is a bunch of zeros
    DACE::AlgebraicVector<double> zeroed(n);

    // Insert nominal in the last position
    this->insert_nominal(zeroed);
}

void delta::insert_nominal(const DACE::AlgebraicVector<double>& n)
{
    // Insert nominal in the last position
    this->scv_deltas_->push_back(n);

    // Set boolean to true
    this->zeroed_inserted_ = true;
}

void delta::set_bool_option(DELTA_GENERATOR_OPTION option, bool value)
{
    // If case relying on the option
    if (option == DELTA_GENERATOR_OPTION::ATTITUDE)
    {
        this->attitude_ = value;
    }
    else if (option == DELTA_GENERATOR_OPTION::QUAT2EULER)
    {
        this->quat2euler_ = value;
    }
}


void delta::attitude_safety_checks()
{
    // Safety checks for attitude

    // Check mean quaternion
    if (this->mean_quaternion_.empty())
    {
        // Throw error
        std::fprintf(stderr, "FATAL: Main quaternion has to be passed to 'delta' class.");

        // Exit program
        std::exit(-1);
    }
}

void delta::convert_non_eval_deltas_to_euler()
{
    // Auxiliary variable
    DACE::AlgebraicVector<double> q;
    DACE::AlgebraicVector<double> euler;

    for (auto & delta : *this->scv_deltas_)
    {
        // Get quaternion
        q = delta.extract(0, 3);

        // Sum up first position
        q[0] += 1;

        // Compute euler angle
        euler = quaternion::quaternion2euler(q);

        // Set new delta
        delta = {euler[0] + this->mean_euler_[0],
                euler[1] + this->mean_euler_[1],
                euler[2] + this->mean_euler_[2],
                delta[4], delta[5], delta[6]};
    }
}

void delta::set_mean_quaternion_option(std::vector<double> mean_q)
{
    // Mean quaternion
    this->mean_quaternion_ = std::move(mean_q);

    // From there, compute euler angles
    this->mean_euler_ = quaternion::quaternion2euler(this->mean_quaternion_);

}
