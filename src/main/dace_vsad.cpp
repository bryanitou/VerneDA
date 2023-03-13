/**
 * DACE_VSOD (VERY SIMPLE ATTITUDE DETERMINATION): aims to solve a problem where the initial orientation is known and
 * we want to compute the future attitude of the spacecraft after a given time using DACE.
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
#include "solids/solid.h"
#include "quaternion.h"

/**
 * Main entry point
 */
int main(int argc, char* argv[])
{
    // Initialize DACE with 6 variables
    DACE::DA::init(2, 7);

    // Declare a geometric figure: cylinder
    auto upper_base_center = point(0.0, 0.0, 2.0);
    auto lower_base_center = point(0.0, 0.0, -2.0);
    std::vector<point> point_list = {upper_base_center, lower_base_center};
    auto cylinder = std::make_unique<solid>();
    cylinder->addpoints(point_list);
    double radius = 2.0;

    // Get initial quaternion
    auto q_control = quaternion::get_from_Euler(0.0, 0.0, 0.0);

    // Set error
    double error = 10.0;

    // Declare the state control vector
    std::vector<DACE::DA> scv0 = {
            q_control[0], // q0
            q_control[1], // q1
            q_control[2], // q2
            q_control[3], // q3
            100.0, // w1 -> rotation around 1 axis
            100.0, // w2 -> rotation around 2 axis
            0.0  };  // w3 -> rotation around 3 axis

    // Declare and initialize class
    auto s0 = std::make_unique<scv>(scv0, true, error);

    // Now, should initialize all the dace variables from the initial conditions
    auto scv0_DA = s0->get_state_vector_copy();

    // Initial and final time
    // Initial time is zero
    double const t0 = 0.0;

    // How many periods do we want to integrate?
    double const tf = 1000;

    // Initialize integrator
    auto objIntegrator = std::make_unique<integrator>(INTEGRATOR::RK4, 1);

    // Define problem to solve
    auto attitudeProblem = reinterpret_cast<DACE::AlgebraicVector<DACE::DA> (*)(DACE::AlgebraicVector<DACE::DA>, double)>(&problems::Attitude);

    // Apply integrator
    auto xf_DA =
            objIntegrator->integrate(scv0_DA, attitudeProblem, t0, tf);

    // Now we have to evaluate the deltas (little displacements in the initial position)
    auto scvf_DA = std::make_shared<scv>(xf_DA);

    // Build deltas class
    auto deltas_engine = std::make_shared<delta>(*scvf_DA, xf_DA);

    // Set distribution
    deltas_engine->set_constants(error, 1.0, error, 1.0);

    // Compute deltas
    deltas_engine->compute_deltas(DISTRIBUTION::GAUSSIAN, 10000, STATE::PX, true);

    // Set output path
    std::filesystem::path output_path_avd = "./out/attp4/taylor_expression_RK4.avd";
    std::filesystem::path output_path_dd = "./out/attp4/deltas_expression_RK4.dd";

    // Dump final info
    tools::io::dace::dump_algebraic_vector(xf_DA, output_path_avd);
    tools::io::dace::dump_deltas(deltas_engine.get(), output_path_dd, true);

    // Draw plots
    tools::io::plot_variables(output_path_dd, PYPLOT_BANANA);
}