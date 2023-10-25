/**
 * DACE_VSOD (VERY SIMPLE ATTITUDE DETERMINATION): aims to solve a problem where the initial orientation is known and
 * we want to compute the future attitude_ of the spacecraft after a given time using DACE.
 */
// DACE libraries
#include "dace/dace.h"

// Project libraries
#include "scv.h"
#include "integrator.h"
#include "problems.h"
#include "delta.h"
#include "json/json_parser.h"
#include "specs/args_input.h"
#include "writer.h"
#include "FileProcessor.h"

/**
 * Main entry point
 */
int main(int argc, char* argv[]) {
    // Build compilation info header
    auto header_info = HeaderInfo(PROGRAM_NAME, CODE_VERSION, GIT_BRANCH, GIT_HASH,
                                  USER_NAME, OS_VERSION, __DATE__, __TIME__);

    // Parse arguments
    auto args_in = args_input(argc, argv);

    // Process arguments
    args_in.run_arguments(header_info);

    // Print ASCII BANNER
    header_info.print_header_info();

    // Remove warnings
    DACE::DACEException::setWarning(false);

    // Create my_specs object
    auto my_specs = json_parser::parse_input_file(args_in.json_filepath);

    // Initialize DACE with 6 variables
    DACE::DA::init(my_specs.algebra.order, my_specs.algebra.variables);

    /* TODO: Check why this was done for ADS...
    auto q_errToll = quaternion::euler2quaternion(
            my_specs.ads.tolerance[0],
            my_specs.ads.tolerance[1],
            my_specs.ads.tolerance[2]);
    q_errToll[0] = INFINITY; //1 - q_errToll[0];

    // Create a vector of all the tolerances
    auto error_tolerance = std::vector<double>(q_errToll.begin(), q_errToll.end());
    error_tolerance.insert(error_tolerance.end(), my_specs.ads.tolerance.begin() + 3, my_specs.ads.tolerance.end());
    */

    // Declare the state control vector with DA
    DACE::AlgebraicVector<DACE::DA> scv0 = {
            my_specs.initial_conditions.mean[0] + my_specs.scaling.beta[0] * DACE::DA(1), // q0
            my_specs.initial_conditions.mean[1] + my_specs.scaling.beta[1] * DACE::DA(2), // q1
            my_specs.initial_conditions.mean[2] + my_specs.scaling.beta[2] * DACE::DA(3), // q2
            my_specs.initial_conditions.mean[3] + my_specs.scaling.beta[3] * DACE::DA(4), // q3
            my_specs.initial_conditions.mean[4] + my_specs.scaling.beta[4] * DACE::DA(5), // w1
            my_specs.initial_conditions.mean[5] + my_specs.scaling.beta[5] * DACE::DA(6), // w2
            my_specs.initial_conditions.mean[6] + my_specs.scaling.beta[6] * DACE::DA(7)  // w3
    };

    std::cout << scv0 << std::endl;

    // Declare and initialize class
    auto s0 = std::make_unique<scv>(scv0);

    // Now, should initialize all the dace variables from the initial conditions
    auto scv0_DA = s0->get_state_vector_copy();

    // Initial and final time and time step
    double const t0 = my_specs.propagation.initial_time;
    double const tf = my_specs.propagation.final_time;
    double const dt = my_specs.propagation.time_step;

    // Initialize integrator
    auto objIntegrator = std::make_unique<integrator>(my_specs.propagation.integrator, my_specs.algorithm, dt);

    // Deduce whether interruption feature shall be made or not
    bool interruption = false;

    // Define problem to solve
    problems *prob;

    // Create an empty SuperManifold to be filled in the switch case
    SuperManifold *super_manifold;

    // Build super manifold
    switch (my_specs.algorithm) {
        case ALGORITHM::ADS: {
            // Build super manifold: ADS
            super_manifold = new SuperManifold(my_specs.ads.tolerance, my_specs.ads.max_split[0], ALGORITHM::ADS);

            // Define problem
            prob = new problems(my_specs.problem, my_specs.mu);

            // Deduce whether interruption should be made or not
            interruption = !my_specs.ads.max_split.empty() && my_specs.ads.max_split[0] > 0;

            // Exit switch case
            break;
        }
        case ALGORITHM::LOADS: {
            // Build super manifold: LOADS
            super_manifold = new SuperManifold(my_specs.loads.nli_threshold, my_specs.loads.max_split[0],
                                               ALGORITHM::LOADS);

            // Set beta constant in integrator
            objIntegrator->set_beta(my_specs.scaling.beta);

            // Set time scaling
            objIntegrator->set_time_scaling(my_specs.scaling.time);

            // Initialize problem
            prob = new problems(my_specs.problem, my_specs.mu);

            // Deduce whether interruption should be made or not
            interruption = !my_specs.loads.max_split.empty() && my_specs.loads.max_split[0] > 0;

            // Exit switch case
            break;
        }
        case ALGORITHM::NONE: {
            // NONE algorithm is equivalent to Splitting algorithms but with interruption mode DISABLED
            super_manifold = new SuperManifold(ALGORITHM::NONE);

            // Initialize problem
            prob = new problems(my_specs.problem, my_specs.mu);

            // Exit switch case
            break;
        }
        default:
        {
            // TODO: Show some errors here
            std::exit(20);
        }
    }

    // Set the inertia matrix in problem object
    prob->set_inertia_matrix(my_specs.initial_conditions.inertia);

    // Set problem ptr in the integrator
    objIntegrator->set_problem_ptr(prob);

    // Setting integrator parameters
    objIntegrator->set_integration_parameters(scv0_DA, t0, tf, interruption);

    // Set integrator in the super manifold
    super_manifold->set_integrator_ptr(objIntegrator.get());

    // Docu: Set new truncation error and get the previous one
    double new_eps = 1e-40;
    double previous_eps = DACE::DA::setEps(new_eps);

    // Show to the used the new epsilon value
    std::fprintf(stdout, "Epsilon update: Previous: '%1.16f', New: '%1.16f'\n", previous_eps, new_eps);

    // ADS and integration algorithm
    super_manifold->split_domain();

    // Convert resulting manifold to 6 variable
    super_manifold->set_6dof_domain();

    std::cout << (*super_manifold->get_att6dof_fin())[0].toString() << std::endl;
    std::cout << (*super_manifold->get_att6dof_ini())[0].toString() << std::endl;

    // Build deltas class
    auto deltas_engine = std::make_shared<delta>();

    // Set some options
    deltas_engine->set_bool_option(DELTA_GENERATOR_OPTION::ATTITUDE, true);
    deltas_engine->set_bool_option(DELTA_GENERATOR_OPTION::QUAT2EULER, true);
    deltas_engine->set_sampling_option(QUATERNION_SAMPLING::OMPL_GAUSSIAN);
    deltas_engine->set_mean_quaternion_option(scv0.extract(0, 3).cons());

    // Set distribution
    deltas_engine->set_stddevs(my_specs.initial_conditions.standard_deviation);

    // Compute deltas
    deltas_engine->generate_deltas(DISTRIBUTION::GAUSSIAN, 10000);

    // Insert nominal delta
    deltas_engine->insert_nominal(static_cast<int>(scv0_DA.size()));

    // Set super manifold in deltas engine
    deltas_engine->set_superManifold(super_manifold);

    // Evaluate deltas
    deltas_engine->evaluate_deltas();

    // Once evaluated, convert initial domain to euler angles, just for plotting stuff
    deltas_engine->convert_non_eval_deltas_to_euler();

    // Create writer object to write files
    writer writer{};

    // What to write
    writer.set_dump_nominal_results(true, true);
    writer.set_dump_centers_results(false);
    writer.set_dump_walls_results(false);

    // Write files
    writer.write_files(deltas_engine.get(), my_specs.output_dir);

    // Create post-processing object
    FileProcessor fproc(writer.get_out_obj());
    fproc.set_metrics(my_specs.initial_conditions.length_units);

    // Set UCFLAGS
    fproc.set_ucflags(PYPLOT_ATTITUDE, PYPLOT_BANANA);

    // Process files
    fproc.process_files();

    // TODO: Destroy here all the stuff to avoid mem-leaks
}