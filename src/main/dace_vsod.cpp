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

/**
 * Main entry point
 */
int main(int argc, char* argv[])
{
    // Initialize DACE with 6 variables
    DACE::DA::init(2, 6);

    // Define some constants
    double const ecc = 0.0;
    double const alt = 300E3; // 300 km of altitude
    double const a = constants::earth::radius + alt; // 300 Km of altitude
    double const vy = sqrt(constants::earth::mu / a) * sqrt(1 + ecc);

    // Set error
    double error = 100.0;

    // Set initial state
    std::vector<DACE::DA> scv0 = {
            a,      //
            0.0,    //
            0.0,    //
            0.0,    //
            vy,     //
            0.0};   //

    // Declare and initialize class
    auto s0 = std::make_unique<scv>(scv0, true, error);

    // Now, should initialize all the dace variables from the initial conditions
    auto scv0_DA = s0->get_state_vector_copy();

    // Initial and final time
    // Initial time is zero
    double const t0 = 0.0;

    // Compute the time for one period
    double const rev = 2*M_PI*std::sqrt(a*a*a/constants::earth::mu);

    // How many periods do we want to integrate?
    double const tf = rev*10;

    // Initialize integrator
    auto objIntegrator = std::make_unique<integrator>(INTEGRATOR::RK78, 1);

    // Define problem to solve
    auto twoBodyProblem = reinterpret_cast<DACE::AlgebraicVector<DACE::DA> (*)(DACE::AlgebraicVector<DACE::DA>, double)>(&problems::TwoBodyProblem);

    // Apply integrator
    auto xf_DA = objIntegrator->integrate(scv0_DA, twoBodyProblem, t0, tf);

    // Now we have to evaluate the deltas (little displacements in the initial position)
    auto scvf_DA = std::make_shared<scv>(xf_DA);

    // Build deltas class
    auto deltas_engine = std::make_shared<delta>(*scvf_DA, xf_DA);

    // Set distribution
    deltas_engine->set_constants(error, 10.0, error, 10.0);

    // Compute deltas
    deltas_engine->compute_deltas(DISTRIBUTION::GAUSSIAN, 10000, STATE::PX);

    // Set output path
    std::filesystem::path output_path_avd = "./out/tbp2/taylor_expression_RK4.avd";
    std::filesystem::path output_path_dd = "./out/tbp2/deltas_expression_RK4.dd";

    // Dump final info
    tools::io::dace::dump_algebraic_vector(xf_DA, output_path_avd);
    tools::io::dace::dump_deltas(deltas_engine.get(), output_path_dd);

    // Draw plots
    tools::io::plot_variables(output_path_dd, PYPLOT_BANANA, "m");

}
