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
void dump_points(DACE::DA &x, DACE::DA &y, const std::string & file_path);

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

    //

    // Print x and y to screen
    std::cout << "x" << std::endl << x << std::endl;
    std::cout << "y = sin(x)" << std::endl << y;

    dump_points(x, y, "./out/test.txt");
}

void dump_points(DACE::DA &x, DACE::DA &y, const std::string & file_path)
{
    // Create the file stream
    std::ofstream vertexes_vrt_stream;
    vertexes_vrt_stream.open(file_path);

    // Safety check
    bool same_length = x.size() == y.size();

    // Check bool
    if (same_length) {
        // Do something here
    }

    vertexes_vrt_stream << x << std::endl;
    vertexes_vrt_stream << "---------------------" << std::endl;
    vertexes_vrt_stream << y << std::endl;

    vertexes_vrt_stream.close();

    // Iterate through the number of vertices
   //  for (int i = 0; i < x.size(); i++) {
   //      // Write in the stream
   //      vertexes_vrt_stream << std::to_string(i) << ":" << x << ", " << y << " " << std::endl;
   //      vertexes_vrt_stream << std::to_string(i) << ":" << x << ", " << y << " " << std::endl;
   //  }
}