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
    // Initialize DACE for 20th-order computations in 1 variable
    DACE::DA::init( 20, 1 );

    // Initialize x as DA
    DACE::DA x = DACE::DA(1);

    // Compute y = sin(x)
    DACE::DA y = DACE::sin(x);

    // Print x and y to screen
    std::cout << "x" << std::endl << x << std::endl;
    std::cout << "y = sin(x)" << std::endl << y;

    dump_variables(x, y, "./out/Example1-dace_test.txt");
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
    file2write << "x" << std::endl << x << std::endl;
    file2write << "y = sin(x)" << std::endl << y;

    // TODO: Try to print coef, exponents (and order) in a organized way so we can print this using Taylor Expansions
    // later in python?
    for (int i = 0; i < y.getMaxOrder())

    // Close the stream
    file2write.close();
}