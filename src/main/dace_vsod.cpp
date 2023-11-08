/**
 * DACE_VSOD (VERY SIMPLE ORBIT DETERMINATION): aims to solve a problem where the initial scv is known and we want to
 * compute the future scv of the spacecraft after a given time using DACE.
 */
// DACE libraries
#include "dace/dace.h"

// Project libraries
#include "base/Header_Info.h"
#include "ads/SuperManifold.h"
#include "tools/io.h"
#include "json/json_parser.h"
#include "specs/args_input.h"
#include "writer.h"
#include "FileProcessor.h"

/**
 * Main entry point
 */
int main(int argc, char* argv[])
{
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

    // Set initial state
    DACE::AlgebraicVector<DACE::DA> scv0 = {
            my_specs.initial_conditions.mean[0] + my_specs.scaling.beta[0] * DACE::DA(1),
            my_specs.initial_conditions.mean[1] + my_specs.scaling.beta[1] * DACE::DA(2),
            my_specs.initial_conditions.mean[2] + my_specs.scaling.beta[2] * DACE::DA(3),
            my_specs.initial_conditions.mean[3] + my_specs.scaling.beta[3] * DACE::DA(4),
            my_specs.initial_conditions.mean[4] + my_specs.scaling.beta[4] * DACE::DA(5),
            my_specs.initial_conditions.mean[5] + my_specs.scaling.beta[5] * DACE::DA(6)
    };

    std::cout << scv0 << std::endl;

    // Initial and final time and time step
    double const t0 = my_specs.propagation.initial_time;
    double const tf = my_specs.propagation.final_time;
    double const dt = my_specs.propagation.time_step;

    // Initialize integrator
    auto objIntegrator = std::make_unique<integrator>(my_specs.propagation.integrator, my_specs.algorithm, dt);

    // Deduce whether interruption feature shall be made or not
    bool interruption = false;

    // Define problem to solve
    problems* prob;

    // Create an empty SuperManifold to be filled in the switch case
    SuperManifold* super_manifold;

    // Build super manifold
    switch (my_specs.algorithm)
    {
        case ALGORITHM::ADS:
        {
            // Build super manifold: ADS
            super_manifold = new SuperManifold(my_specs.ads.tolerance,my_specs.ads.max_split[0], ALGORITHM::ADS);

            // Define problem
            prob = new problems(my_specs.problem, my_specs.mu);

            // Deduce whether interruption should be made or not
            interruption = !my_specs.ads.max_split.empty() && my_specs.ads.max_split[0] > 0;

            // Exit switch case
            break;
        }
        case ALGORITHM::LOADS:
        {
            // Build super manifold: LOADS
            super_manifold = new SuperManifold(my_specs.loads.nli_threshold, my_specs.loads.max_split[0], ALGORITHM::LOADS);

            // Set beta constant in integrator
            objIntegrator->set_beta(my_specs.scaling.beta);

            // Initialize problem
            prob = new problems(my_specs.problem, my_specs.mu);

            // Deduce whether interruption should be made or not
            interruption = !my_specs.loads.max_split.empty() && my_specs.loads.max_split[0] > 0;

            // Exit switch case
            break;
        }
        case ALGORITHM::NONE:
        {
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

    // Set problem ptr in the integrator
    objIntegrator->set_problem_ptr(prob);

    // Setting integrator parameters
    objIntegrator->set_integration_parameters(scv0, t0, tf,interruption);

    // Set integrator in the super manifold
    super_manifold->set_integrator_ptr(objIntegrator.get());

    // Docu: Set new truncation error and get the previous one
    double new_eps = 1e-40;
    double previous_eps = DACE::DA::setEps(new_eps);

    // Show to the used the new epsilon value
    std::fprintf(stdout, "Epsilon update: Previous: '%1.16f', New: '%1.16f'\n", previous_eps, new_eps);

    // Apply main algorithm: ADS / LOADS. And integration algorithm
    super_manifold->split_domain();

    // Build deltas class
    auto deltas_engine = std::make_shared<delta>();

    // Set distribution
    deltas_engine->set_stddevs(my_specs.initial_conditions.standard_deviation);

    // Compute deltas
    deltas_engine->generate_deltas(DISTRIBUTION::GAUSSIAN, 10000);

    // Insert nominal delta
    deltas_engine->insert_nominal(my_specs.algebra.variables);

    // Set super manifold in deltas engine
    deltas_engine->set_superManifold(super_manifold);

    // Evaluate deltas
    deltas_engine->evaluate_deltas();

    // Create writer object to write files
    writer writer{};

    // What to write
    writer.set_dump_nominal_results(true, true);
    // writer.set_dump_frames_results(true, true);

    // Write files
    writer.write_files(deltas_engine.get(), my_specs.output_dir);

    // Create post-processing object
    FileProcessor fproc(writer.get_out_obj());
    fproc.set_metrics(my_specs.initial_conditions.length_units);

    // Set UCFLAGS
    fproc.set_ucflags(PYPLOT_TRANSLATION, PYPLOT_BANANA);

    // Process files
    fproc.process_files();

    // TODO: Destroy here all the stuff to avoid mem-leaks
}
