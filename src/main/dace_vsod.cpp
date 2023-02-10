/**
 * DACE_VSOD (VERY SIMPLE ORBIT DETERMINATION): aims to solve a problem where the initial state is known and we want to
 * compute the future state of the spacecraft after a given time using DACE with conjunction of RK4.
 */
// DACE libraries
#include "dace/dace.h"

// Project libraries
#include "state.h"
#include "integrator.h"

/**
 * Main entry point
 */
int main(int argc, char* argv[])
{
    // Initialize DACE with 6 variables
    DACE::DA::init(1, 6);

    // Declare state class
    std::unique_ptr<state> s0 = std::make_unique<state>();

    // Define some constants
    double const ecc = 0.5;
    double const a = 6678.0; // 300 Km of altitude
    double const mu = 398600; // km^3 / s^2
    double const vy = sqrt(mu / a) * sqrt(1 + ecc);

    // Initialize class
    s0->allocate_state_values(a, 0.0, 0.0, 0.0, vy, 0);

    // Now, should initialize all the dace variables from the initial conditions
    auto s0_DA = s0->get_state_vector();

    // Initial and final time
    double const t0 = 0.0;
    double const tf = M_PI*sqrt(a*a*a/mu);

    // Initialize integrator
    auto my_integrator = std::make_unique<integrator>(INTEGRATOR::EULER);

    // Apply integrator
    auto xf_DA = my_integrator->euler(s0_DA, t0, tf);
}
