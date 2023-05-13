/**
 * DACE_VSOD (VERY SIMPLE ORBIT DETERMINATION): aims to solve a problem where the initial scv is known and we want to
 * compute the future scv of the spacecraft after a given time using DACE.
 */
// DACE libraries
#include "dace/dace.h"

// Project libraries
#include "scv.h"
#include "integrator.h"
#include "base/constants.h"
#include "problems.h"
#include "delta.h"
#include "tools/io.h"
#include "json/json_parser.h"

// ADS library
#include "ads/ads.h"


/**
 * Main entry point
 */
int main(int argc, char* argv[])
{
    // Create my_specs object
    auto my_specs = json_parser::parse_input_file("./cfg/fossa_validation/ads_rk4_io.json");

    // Initialize DACE with 6 variables
    DACE::DA::init(my_specs.algebra.order, my_specs.algebra.variables);

    // Set initial state
    DACE::AlgebraicVector<DACE::DA> scv0 = {
            my_specs.initial_conditions.mean[0] + 3*DACE::DA(1) * my_specs.initial_conditions.standard_deviation[0],
            my_specs.initial_conditions.mean[1] + 3*DACE::DA(2) * my_specs.initial_conditions.standard_deviation[1],
            my_specs.initial_conditions.mean[2] + DACE::DA(3) * my_specs.initial_conditions.standard_deviation[2],
            my_specs.initial_conditions.mean[3] + DACE::DA(4) * my_specs.initial_conditions.standard_deviation[3],
            my_specs.initial_conditions.mean[4] + DACE::DA(5) * my_specs.initial_conditions.standard_deviation[4],
            my_specs.initial_conditions.mean[5] + DACE::DA(6) * my_specs.initial_conditions.standard_deviation[5] };

    // Declare and initialize class
    auto s0 = std::make_unique<scv>(scv0);

    // Now, should initialize all the dace variables from the initial conditions
    auto scv0_DA = s0->get_state_vector_copy();

    // Dump for comparison
    std::ofstream f2w;

    // Open file
    f2w.open("./out/validation/ads_rk4_initial_domain.dat");

    f2w << scv0_DA << std::endl;

    f2w.close();

    // Initial and final time and time step
    double const t0 = my_specs.propagation.initial_time;
    double const tf = my_specs.propagation.final_time;
    double const dt = my_specs.propagation.time_step;

    // Initialize integrator
    auto objIntegrator = std::make_unique<integrator>(INTEGRATOR::RK4, dt);

    // Define problem to solve
    auto prob = problems(PROBLEM::TWO_BODY);

    // Build super manifold
    auto super_manifold = SuperManifold(my_specs.ads.tolerance,
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

    // TODO: Fix this ONWARDS
    auto xf_DA = scv0_DA;

    // Now we have to evaluate the deltas (little displacements in the initial position)
    auto scvf_DA = std::make_shared<scv>(xf_DA);

    // Build deltas class
    auto deltas_engine = std::make_shared<delta>(*scvf_DA, xf_DA);

    // Set distribution
    deltas_engine->set_constants(my_specs.initial_conditions.standard_deviation);

    // Compute deltas
    deltas_engine->generate_deltas(DISTRIBUTION::GAUSSIAN, 10000);

    // Insert nominal delta
    deltas_engine->insert_nominal(my_specs.algebra.variables);

    // Evaluate deltas
    deltas_engine->evaluate_deltas(&super_manifold);

    // Set output path for results
    std::filesystem::path output_dir = "./out/validation";
    std::filesystem::path output_path_avd = output_dir / "taylor_expression_RK4.avd";
    std::filesystem::path output_eval_deltas_path_dd =  output_dir / "eval_deltas_expression_RK4.dd";
    std::filesystem::path output_non_eval_deltas_path_dd =  output_dir / "non_eval_deltas_expression.dd";

    // Dump final info
    tools::io::dace::dump_algebraic_vector(xf_DA, output_path_avd);

    // Dump non evaluated deltas
    tools::io::dace::dump_non_eval_deltas(deltas_engine.get(), output_non_eval_deltas_path_dd);

    // Dump evaluated deltas
    tools::io::dace::dump_eval_deltas(deltas_engine.get(), output_eval_deltas_path_dd);

    // Prepare arguments for python call
    std::unordered_map<std::string, std::string> py_args = {
            {"file", output_eval_deltas_path_dd},
            {"plot_type", PYPLOT_TRANSLATION},
            {"metrics", "m"},
    };

    // Draw plots
    tools::io::plot_variables(PYPLOT_BANANA, py_args, true);

}
