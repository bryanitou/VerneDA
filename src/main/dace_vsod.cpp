/**
 * DACE_VSOD (VERY SIMPLE ORBIT DETERMINATION): aims to solve a problem where the initial scv is known and we want to
 * compute the future scv of the spacecraft after a given time using DACE with conjunction of RK4.
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
    DACE::DA::init(1, 6);

    // Define some constants
    double const ecc = 0.5;
    double const alt = 300E3; // 300 m of altitude
    double const a = constants::earth::radius + alt; // 300 Km of altitude
    double const mu = constants::earth::mu; // km^3 / s^2
    double const vy = sqrt(mu / a) * sqrt(1 + ecc);

    // Declare and initialize class
    auto s0 = std::make_unique<scv>(a, 0.0, 0.0, 0.0, vy, 0);

    // Now, should initialize all the dace variables from the initial conditions
    auto scv0_DA = s0->get_state_vector();

    // Initial and final time
    double const t0 = 0.0;
    double const tf = M_PI*sqrt(a*a*a/mu);

    // Initialize integrator
    auto eulerIntegrator = std::make_unique<integrator>(INTEGRATOR::EULER);

    // Define problem to solve
    auto twoBodyProblem = reinterpret_cast<DACE::AlgebraicVector<DACE::DA> (*)(DACE::AlgebraicVector<DACE::DA>, double)>(&problems::TwoBodyProblem);

    // Apply integrator
    auto xf_DA = eulerIntegrator->euler(scv0_DA, twoBodyProblem, t0, tf);

    // Now we have to evaluate the deltas (little displacements in the initial position)
    auto scvf_DA = std::make_shared<scv>(xf_DA);

    // Build deltas class
    auto deltas_engine = std::make_shared<delta>(*scvf_DA, xf_DA);

    // Compute deltas
    deltas_engine->compute_deltas(DISTRIBUTION::GAUSSIAN, 100);

    // Dump final info
    tools::io::dump_algebraic_vector(xf_DA, "./out/tbp/taylor_expression.te");
    tools::io::dump_deltas(deltas_engine.get(), "./out/tbp/deltas_expression.csv");
}
