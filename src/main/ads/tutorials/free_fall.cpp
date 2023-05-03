/**
 * Tutorial main where ADS is used for a sinus function.
 */

// System libraries


// ADS libraries
#include "ads/ads.h"
#include "problems.h"
#include "integrator.h"

// DACE libraries
#include "dace/dace.h"

int main()
{
    // Set amount of order and variables
    int n_ord = 2;
    int n_var = 3;

    // Initialize order and amount of variables
    DACE::DA::init(n_ord, n_var);

    // Which Epsilon is this one?
    // Docu: Set new truncation error and get the previous one
    double new_eps = 1e-40;
    double previous_eps = DACE::DA::setEps(new_eps);

    // Show to the used the new epsilon value
    std::fprintf(stdout, "Epsilon update: Previous: '%1.16f', New: '%1.16f'\n", previous_eps, new_eps);

    // Do we want to see the warnings? True (1) or False (0)
    DACE::DACEException::setWarning(true);

    // Set the precision of the doubles to printed
    std::cout.precision(10);

    // This block reads info from the txt file that contains observations-----------------------------------------------

    // Initialize AlgebraicVector
    DACE::AlgebraicVector<DACE::DA> rv(6);

    // Set state vector, it is only R1.. so only one position
    rv[0] = 0.0; // Position x
    rv[0] = 0.0; // Position y
    rv[0] = 100.0; // Position z
    rv[0] = 0.01 * DACE::DA(1); // Velocity x
    rv[0] = 0.01 * DACE::DA(1); // Velocity y
    rv[0] = 0.01 * DACE::DA(1); // Velocity z

    // Let's set the function
    auto y = DACE::sin(rv);

    // This is the mu in km^3 / s^
    double mu = 398600;

    // 3.3 days ?, 24 hours and 3600.0 seconds
    double DT = 3.3*24.0*3600.0;

    // Error tolerances and standard deviations Tolerances
    std::vector<double> errToll = {1e-3};

    // Num. split max -> Stands for exactly what?
    // INFO: This value is set as a maximum of splits per step
    int nSplitMax = 6;

    // Initialize patch
    // TODO: This is never used
    Patch set(rv);

    // Initialize manifold
    Manifold stack(rv);

    // Set clock
    // TODO: Unused
    clock_t split_time = clock();

    // Initialize problem
    auto prob = problems(PROBLEM::FREE_FALL_OBJECT);

    // Build and initialize integrator
    auto objIntegrator = std::make_unique<integrator>(INTEGRATOR::RK78, 1);

    // Set the problem into the integrator object
    objIntegrator->set_problem_object(&prob);

    // Call to main running function
    Manifold RVsplit = stack.getSplitDomain(probObj.solve(), errToll, nSplitMax, DT,  mu );

    // Info to the user
    std::fprintf(stdout," It creates '%zu' domains.", RVsplit.size());

    // Close function with exit code 0
    return 0;
}


