/**
 * DACE_VSOD (VERY SIMPLE ORBIT DETERMINATION): aims to solve a problem where the initial scv is known and we want to
 * compute the future scv of the spacecraft after a given time using DACE.
 */
// DACE libraries
#include "dace/dace.h"

// Project libraries
#include "scv.h"
#include "integrator.h"
#include "problems.h"
#include "delta.h"
#include "tools/io.h"
#include "json/json_parser.h"
#include "specs/args_input.h"

// ADS library
#include "ads/ads.h"


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
            prob = new problems(my_specs.problem);

            break;
        }
        case ALGORITHM::LOADS:
        {
            // Build super manifold: LOADS
            super_manifold = new SuperManifold(my_specs.loads.nli_threshold, my_specs.loads.max_split[0], ALGORITHM::LOADS);

            // Set beta constant in integrator
            objIntegrator->set_beta(my_specs.scaling.beta);

            // Set time scaling
            objIntegrator->set_time_scaling(my_specs.scaling.time);

            // Define problem
            prob = new problems(my_specs.problem, my_specs.mu);
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
    objIntegrator->set_integration_parameters(scv0_DA, t0, tf, true);

    // Set integrator in the super manifold
    super_manifold->set_integrator_ptr(objIntegrator.get());

    // Docu: Set new truncation error and get the previous one
    double new_eps = 1e-40;
    double previous_eps = DACE::DA::setEps(new_eps);

    // Show to the used the new epsilon value
    std::fprintf(stdout, "Epsilon update: Previous: '%1.16f', New: '%1.16f'\n", previous_eps, new_eps);

    // ADS and integration algorithm
    super_manifold->split_domain();

    // Build deltas class
    auto deltas_engine = std::make_shared<delta>();

    // Set distribution
    deltas_engine->set_stddevs(my_specs.initial_conditions.standard_deviation);

    // Compute deltas
    deltas_engine->generate_deltas(DISTRIBUTION::GAUSSIAN, 1);

    // Insert nominal delta
    deltas_engine->insert_nominal(my_specs.algebra.variables);

    // Set super manifold in deltas engine
    deltas_engine->set_superManifold(super_manifold);

    // Evaluate deltas
    deltas_engine->evaluate_deltas();

    // Set output path for results
    std::filesystem::path output_dir = my_specs.output_dir;
    std::filesystem::path output_eval_deltas_path_dd =  output_dir / "eval_deltas_expression_RK4.dd";
    std::filesystem::path output_eval_deltas_box_path_dd =  output_dir / "eval_deltas_expression_box_RK4.dd";
    std::filesystem::path output_non_eval_deltas_path_dd =  output_dir / "non_eval_deltas_expression.dd";

    // Some other useful optional outputs: validation
    std::filesystem::path output_walls = output_dir / "eval_walls_RK4.walls";
    std::filesystem::path output_walls_box = output_dir / "eval_walls_box_RK4.walls";
    std::filesystem::path output_centers = output_dir / "eval_centers.dd";
    std::filesystem::path output_centers_box = output_dir / "eval_centers_box.dd";

    // Debugging files
    std::filesystem::path output_debug_splitting_history = output_dir / "splitting_history.txt";

    // Dump non evaluated deltas
    tools::io::dace::dump_non_eval_deltas(deltas_engine.get(), output_non_eval_deltas_path_dd);

    // Dump evaluated deltas
    tools::io::dace::dump_eval_deltas(deltas_engine.get(), output_eval_deltas_path_dd);
    tools::io::dace::dump_eval_deltas(deltas_engine.get(), output_eval_deltas_box_path_dd);

    // Dump eval points at the walls
    tools::io::dace::dump_eval_deltas(deltas_engine.get(), output_walls, EVAL_TYPE::WALLS);
    tools::io::dace::dump_eval_deltas(deltas_engine.get(), output_walls_box, EVAL_TYPE::INITIAL_WALLS);

    // Dump eval points at the center
    tools::io::dace::dump_eval_deltas(deltas_engine.get(), output_centers, EVAL_TYPE::CENTER);
    tools::io::dace::dump_eval_deltas(deltas_engine.get(), output_centers_box, EVAL_TYPE::INITIAL_CENTER);

    // Debugging dumps
    tools::io::dace::dump_splitting_history(deltas_engine.get(), output_debug_splitting_history);

    // Prepare arguments for python call
    std::unordered_map<std::string, std::string> py_args = {
            {"file", output_eval_deltas_box_path_dd},
            {"plot_type", PYPLOT_TRANSLATION},
            {"metrics", "m"},
            {"centers", output_centers_box},
            {"walls", output_walls_box},
            {"silent", "false"}
    };

    // Draw plots
    tools::io::plot_variables(PYPLOT_BANANA, py_args, false);

}
