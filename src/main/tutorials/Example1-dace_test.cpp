/**
 * First c++ main to interact with the 3rdparty.
 */

// System libraries
#include <iostream>
#include <filesystem>

// DACE library
#include "dace/dace.h"

// Project libraries
#include "tools/io.h"

/**
 * Main entry point
 */
int main([[maybe_unused]] int argc, [[maybe_unused]] char* argv[])
{
    // Initialize DACE for 20th-order computations in 1 variable
    DACE::DA::init( 20, 1 );

    // Initialize x as DA
    DACE::DA x = DACE::DA(1);

    // Compute y = sin(x)
    DACE::DA y = DACE::sin(x);

    // Analytical form
    std::string func_form = "y = sin(x)";
    std::string var_form = "x";

    // Print x and y to screen
    std::cout << var_form << std::endl << x << std::endl;
    std::cout << func_form << std::endl << y;

    // Some pre-set paths
    std::filesystem::path output_path = "./out/Example1-dace_test.txt";

    // Dump variables
    tools::io::dace::dump_variables(y, x, func_form, var_form, output_path);

    // Prepare arguments for python call
    std::unordered_map<std::string, std::string> py_args = {
            {"file", output_path},
            {"span", "5"},
            {"silent", "false"},
    };

    // Make plot
    tools::io::plot_variables(PYPLOT_SIMPLE, py_args, true);

}