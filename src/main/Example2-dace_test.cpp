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


// Forward declarate stuff
void dump_variables(DACE::DA &x, DACE::DA &y, const std::string & file_path);

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

    dump_variables(x, y1, "./out/Example2-dace_test.txt");
}

void dump_variables(DACE::DA &x, DACE::DA &y, const std::string & file_path)
{
    // Create the file stream
    std::ofstream file2write;
    file2write.open(file_path);

    // Safety check
    bool same_length = x.size() == y.size();

    // Check bool
    if (same_length) {
        // Do something here
    }

    // Easy print
    // file2write << "x" << std::endl << x << std::endl;
    file2write << "y = sin(x)^2" << std::endl << y.;

    // Close the stream
    file2write.close();
}