/**
 * First c++ main to interact with the 3rdparty.
 */
#pragma once

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
    // Initialize DACE for 20th-order computations in 1 variable
    // TOASK: I don't clearly understand the ORDER value hereafter
    DACE::DA::init(1, 1);

    // Initialize x as DA
    DACE::DA x = DACE::sqr(DACE::DA(1));
    //x = x*x;
    std::string var_form = "x";

    // Compute [cos(x)-1]
    DACE::DA y = cos(x)-1;
    std::string func_form = "f(x) = [cos(x)-1]";

    std::cout << func_form << std::endl << y << std::endl;

    // Some pre-set paths
    std::filesystem::path output_path = "./out/Example4-dace_test.txt";

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

    // Compute [cos(x)-1]^11
    for ( int i = 0; i < 10; i++)
    {
        y = y*(cos(x)-1);
        std::cout << "f(x) = [cos(x)-1]^" + std::to_string(i+2) << std::endl << y << std::endl;
    }



}