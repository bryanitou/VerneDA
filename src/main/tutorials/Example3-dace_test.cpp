/**
 * First c++ main to interact with the 3rdparty.
 */

// System libraries
#include <iostream>

// DACE library
#include "dace/dace.h"

// Project libraries
#include "tools/io.h"

/**
 * Main entry point
 */
int main([[maybe_unused]] int argc, [[maybe_unused]] char* argv[])
{
    // Initialize DACE for 1st-order computations in 1 variable
    DACE::DA::init(1, 1 );

    // Initialize x as DA around 3
    DACE::DA x1 = 3 + DACE::DA(1);
    DACE::DA x = x1;

    std::string var_form = "x";
    std::cout << var_form << std::endl << x << std::endl;

    // Evaluate f(x) = 1/(x+1/x)
    DACE::DA f = 1/(x+1/x);

    // Analytical form
    std::string func_form = "f(x) = 1/(x+1/x)";

    // Print by terminal
    std::cout << func_form << std::endl << f << std::endl;

    // Some pre-set paths
    std::filesystem::path output_path = "./out/Example3-dace_test.txt";

    // Dump variables
    tools::io::dace::dump_variables(f, x, func_form, var_form, output_path);

    // Prepare arguments for python call
    std::unordered_map<std::string, std::string> py_args = {
            {"file", output_path},
            {"span", "5"},
            {"silent", "false"},
    };

    // Make plot
    tools::io::plot_variables(PYPLOT_SIMPLE,py_args,true);
}
