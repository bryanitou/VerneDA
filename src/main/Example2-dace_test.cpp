/**
 * First c++ main to interact with the 3rdparty.
 */

// System libraries
#include <iostream>

// DACE library
#include <dace/dace.h>

// Project libraries
#include "tools.h"

/**
 * Main entry point
 */
int main([[maybe_unused]] int argc, [[maybe_unused]] char* argv[])
{
    // Init variable, 20-th computations and 1 variable
    DACE::DA::init( 20, 1 );

    // Initialize x as DA
    DACE::DA x = DACE::DA(1);

    // Compute sin(x)^2
    DACE::DA y1 = DACE::sqr(DACE::sin(x));
    std::string func_form1 = "y = sin(x)^2";

    // Info
    std::cout << func_form1 << std::endl << y1 << std::endl;

    // Compute cos(x)^2
    DACE::DA y2 = DACE::sqr(cos(x));
    std::string func_form2 = "y = cos(x)^2";

    // Info
    std::cout << func_form2 << std::endl << y2 << std::endl;

    // Compute and print sin(x)^2+cos(x)^2
    auto y1_y2 = y1 + y2;
    std::string func_form3 = "y = sin(x)^2 + cos^2(x)";
    std::cout << func_form3 << std::endl << y1_y2 << std::endl;

    // Output paths
    std::filesystem::path y1_output_path = "./out/Example2-dace_test_sin2.txt";
    std::filesystem::path y2_output_path = "./out/Example2-dace_test_cos2.txt";
    std::filesystem::path y1_y2_output_path = "./out/Example2-dace_test_sin2cos2.txt";

    // Print all this stuff
    tools::dump_variables(y1, func_form1, y1_output_path);
    tools::dump_variables(y2, func_form2, y2_output_path);
    tools::dump_variables(y1_y2, func_form3, y1_y2_output_path);

    // Plot all this stuff
    tools::plot_variables(y1_output_path, PYTHON_PLOTTER, true);
    tools::plot_variables(y2_output_path, PYTHON_PLOTTER, true);
}