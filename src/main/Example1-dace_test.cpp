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

/**
 * Main entry point
 */
int main(int argc, char* argv[])
{
    DACE::DA::init( 20, 1 ); // initialize DACE for 20th-order computations in 1 variable

    DACE::DA x = DACE::DA(1); // initialize x as DA

    DACE::DA y = sin(x); // compute y = sin(x)

    // print x and y to screen
    std::cout << "x" << std::endl << x << std::endl;
    std::cout << "y = sin(x)" << std::endl << y;
}