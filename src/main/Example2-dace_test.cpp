/**
 * First c++ main to interact with the 3rdparty.
 */

// System libraries
#include <cmath>
#include <iostream>
#include <fstream>
#include <iomanip>

// DACE library
#include <dace/dace.h>

// Project libraries
#include "tools.h"

/**
 * Main entry point
 */
int main(int argc, char* argv[])
{
    // Init variable, 20-th computations and 1 variable
    DACE::DA::init( 20, 1 );

    // Initialize x as DA
    DACE::DA x = DACE::DA(1);

    // Compute sin(x)^2
    DACE::DA y1 = DACE::sqr(DACE::sin(x));

    // Info
    std::cout << "sin(x)^2" << std::endl << y1 << std::endl;

    // Compute cos(x)^2
    DACE::DA y2 = DACE::sqr(cos(x));

    // Info
    std::cout << "cos(x)^2" << std::endl << y2 << std::endl;

    // Compute and print sin(x)^2+cos(x)^2
    std::cout << "sin(x)^2+cos(x)^2" << std::endl << y1+y2 << std::endl;

    tools::dump_variables(y1, "./out/Example2-dace_test.txt");
}