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
#include "tools/io.h"
#include "json/json_parser.h"
#include "specs/args_input.h"
#include "solids/solid.h"

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

    // Get initial quaternion
    // TODO: Error should be computed here
    auto q_init = quaternion::euler2quaternion(
            my_specs.initial_conditions.mean[0],
            my_specs.initial_conditions.mean[1],
            my_specs.initial_conditions.mean[2]);

    auto q_err = quaternion::euler2quaternion(my_specs.initial_conditions.standard_deviation[0],
                                              my_specs.initial_conditions.standard_deviation[1],
                                              my_specs.initial_conditions.standard_deviation[2]);

    auto q_errToll = quaternion::euler2quaternion(
            my_specs.ads.tolerance[0],
            my_specs.ads.tolerance[1],
            my_specs.ads.tolerance[2]);
    q_errToll[0] = INFINITY; //1 - q_errToll[0];

    // Create a vector of all the tolerances
    auto error_tolerance = std::vector<double>(q_errToll.begin(), q_errToll.end());
    error_tolerance.insert(error_tolerance.end(), my_specs.ads.tolerance.begin() + 3, my_specs.ads.tolerance.end());

    // Declare the state control vector with DA
   DACE::AlgebraicVector<DACE::DA> scv0 = {
            q_init[0] + q_err[0] * my_specs.initial_conditions.confidence_interval[0] * DACE::DA(1),       // q0
            q_init[1] + q_err[1] * my_specs.initial_conditions.confidence_interval[1] * DACE::DA(2),       // q1
            q_init[2] + q_err[2] * my_specs.initial_conditions.confidence_interval[2] * DACE::DA(3),       // q2
            q_init[3] + q_err[3] * my_specs.initial_conditions.confidence_interval[3] * DACE::DA(4),       // q3
            my_specs.initial_conditions.mean[3] +  my_specs.initial_conditions.standard_deviation[3] * my_specs.initial_conditions.confidence_interval[3] * DACE::DA(5),     // w1 -> rotation around 1 axis
            my_specs.initial_conditions.mean[4] +  my_specs.initial_conditions.standard_deviation[4] * my_specs.initial_conditions.confidence_interval[4] * DACE::DA(6),     // w2 -> rotation around 2 axis
            my_specs.initial_conditions.mean[5] +  my_specs.initial_conditions.standard_deviation[5] * my_specs.initial_conditions.confidence_interval[5] * DACE::DA(7) };   // w3 -> rotation around 3 axis

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
    auto objIntegrator = std::make_unique<integrator>(my_specs.propagation.integrator, dt);

    // Declare the problem object
    auto prob = problems(my_specs.problem);

    // Set the inertia matrix in problem object
    prob.set_inertia_matrix(my_specs.initial_conditions.inertia);

    // Build super manifold
    auto super_manifold = SuperManifold(error_tolerance,
                                        my_specs.ads.max_split[0]);

    // Set problem ptr in the integrator
    objIntegrator->set_problem_ptr(&prob);

    // Setting integrator parameters
    objIntegrator->set_integration_parameters(scv0_DA, t0, tf, true);

    // Set integrator in the super manifold
    super_manifold.set_integrator_ptr(objIntegrator.get());

    // Docu: Set new truncation error and get the previous one
    double new_eps = 1e-40;
    double previous_eps = DACE::DA::setEps(new_eps);

    // Show to the used the new epsilon value
    std::fprintf(stdout, "Epsilon update: Previous: '%1.16f', New: '%1.16f'\n", previous_eps, new_eps);

    // ADS and integration algorithm
    super_manifold.split_domain();

    // Build deltas class
    auto deltas_engine = std::make_shared<delta>();

    // Set some options
    deltas_engine->set_bool_option(DELTA_GENERATOR_OPTION::ATTITUDE, true);
    deltas_engine->set_bool_option(DELTA_GENERATOR_OPTION::QUAT2EULER, true);
    deltas_engine->set_sampling_option(QUATERNION_SAMPLING::OMPL_GAUSSIAN);
    deltas_engine->set_mean_quaternion_option(quaternion::euler2quaternion(0.0, 0.0, 0.0));

    // Set distribution
    deltas_engine->set_stddevs(my_specs.initial_conditions.standard_deviation);

    // Compute deltas
    deltas_engine->generate_deltas(DISTRIBUTION::GAUSSIAN, 10000);

    // Insert nominal delta
    deltas_engine->insert_nominal(my_specs.algebra.variables);

    // Set super manifold in deltas engine
    deltas_engine->set_superManifold(&super_manifold);

    // Evaluate deltas
    deltas_engine->evaluate_deltas();

    // Set output path for results
    std::filesystem::path output_dir = my_specs.output_dir;
    std::filesystem::path output_eval_deltas_path_dd = output_dir / "eval_deltas_expression_RK4.dd";
    std::filesystem::path output_non_eval_deltas_path_dd = output_dir /  "non_eval_deltas_expression.dd";

    // Some other useful optional outputs: validation
    std::filesystem::path output_walls = output_dir / "eval_walls_RK4.walls";
    std::filesystem::path output_centers = output_dir / "eval_centers.dd";

    // Dump non evaluated deltas
    tools::io::dace::dump_non_eval_deltas(deltas_engine.get(), output_non_eval_deltas_path_dd);

    // Dump evaluated deltas
    tools::io::dace::dump_eval_deltas(deltas_engine.get(), output_eval_deltas_path_dd);

    // Dump eval points at the walls
    tools::io::dace::dump_eval_deltas(deltas_engine.get(), output_walls, EVAL_TYPE::WALLS);

    // Dump eval points at the center
    tools::io::dace::dump_eval_deltas(deltas_engine.get(), output_centers, EVAL_TYPE::CENTER);

    // Prepare arguments for python call
    std::unordered_map<std::string, std::string> py_args = {
            {"file", output_eval_deltas_path_dd},
            {"plot_type", PYPLOT_ATTITUDE},
            {"metrics", "rad"},
            {"centers", output_centers},
            {"walls", output_walls},
            {"silent", "false"}
    };


    // Draw plots
    tools::io::plot_variables(PYPLOT_BANANA, py_args, false);
}