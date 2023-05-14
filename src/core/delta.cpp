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

void delta::generate_deltas(DISTRIBUTION type, int n)
{
    // Place for the all safety checks before computing HERE BELOW:
    // Safety check that the deltas are still not generated
    if (this->scv_deltas_ != nullptr) {
        // WARNING: deltas are gonna be replaced
        std::printf("WARNING: Deltas are going to be replaced! I cannot save values");
    }

    // Safety check that the constants are already set
    if (!this->constants_set_) {
        // WARNING! Must set constants for the distribution competition before
        std::printf("WARNING: Deltas are not going to be computed. Need to provide pos/vel: stddev. "
                    "It has not been set yet. Exiting program.");

        // Exit program
        std::exit(-1);
    }

    // Safety checks if it is attitude, since its more complex...
    if (this->attitude_)
    {
        // Call function checker
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
    // Stack results here: no need to initialize!
    std::vector<std::shared_ptr<scv>> deltas;

    // Compute deltas: by hard set values
    // n: experiments
    // pos: position (x, y, z) that will be modified
    // Call to random engine generator
    std::default_random_engine generator;
    // todo: the mean is the initial condition, see photo

    std::vector< std::normal_distribution<double>> stddevs_distr;

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
        std::vector<DACE::DA> new_delta;
        if (this->attitude_)
        {
            // TODO: To check: KENT DISTRIBUTION OR THE LINK IN quaternions.cpp
            // Declare new quaternion
            std::vector<double> nq;

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

            // Safety check for norm
            double nq_norm = quaternion::getnorm(nq);

            // TODO: What should be the trigger for the norm?
            bool is1norm = nq_norm == 1.0;
            if (!is1norm)
            {
                // TODO: Do something here
            }

            new_delta = {
                    nq[0] - this->mean_quaternion_[0],
                    nq[1] - this->mean_quaternion_[1],
                    nq[2] - this->mean_quaternion_[2],
                    nq[3] - this->mean_quaternion_[3],
                    stddevs_distr[3](generator),
                    stddevs_distr[4](generator),
                    stddevs_distr[5](generator)};
        }
        else
        {
            new_delta = {
                    stddevs_distr[0](generator),
                    stddevs_distr[1](generator),
                    stddevs_distr[2](generator),
                    stddevs_distr[3](generator),
                    stddevs_distr[4](generator),
                    stddevs_distr[5](generator)};
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
    // Safety check
    if (!this->nominal_inserted_)
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
        // single_sol = this->base_poly_->eval(scv_delta->get_state_vector_copy());
        auto sample = scv_delta->get_state_vector_copy().cons();
        auto single_sol = this->sm_->get_final_manifold()->pointEvaluationManifold(
                this->sm_->previous_->front(),
                sample,
                1);

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

            // Convert to Euler
            auto euler_angles = quaternion::quaternion2euler(quaternion[0],
                                                             quaternion[1],
                                                             quaternion[2],
                                                             quaternion[3]);

            if (scv_delta == scv_deltas_->back() && this->attitude_)
            {
                // Some debugging information
                auto scv_cons = scv_delta->get_state_vector_copy().cons();
                auto scv_cons_str = tools::vector::num2string<double>(scv_cons);
                double scv_q_norm = scv_delta->get_state_vector_copy().cons().extract(0, 3).vnorm();
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
            single_sol = {euler_angles[0]  * (180.0 / M_PI),
                          euler_angles[1]  * (180.0 / M_PI),
                          euler_angles[2]  * (180.0 / M_PI), single_sol[4], single_sol[5], single_sol[6]};

            // std::cout << single_sol << std::endl; DEBUG LINE
        }

        // Push back
        taylor_list.emplace_back(single_sol);
    }

    // Make it ptr
    this->eval_deltas_poly_ = std::make_shared<std::vector<DACE::AlgebraicVector<double>>>(taylor_list);
}

void delta::set_constants(std::vector<double> stddevs)
{
    // Set constants
    this->stddevs_ = stddevs;

    // Notice
    this->constants_set_ = true;

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
    DACE::AlgebraicVector<DACE::DA> zeroed(n);
    auto scv_nominal = scv(zeroed);

    // Insert nominal in the last position
    this->scv_deltas_->push_back(std::make_shared<scv>(scv_nominal));

    // Set boolean to true
    this->nominal_inserted_ = true;
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
