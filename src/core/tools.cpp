/**
 *
 */

#include "tools.h"


void tools::dump_variables(DACE::DA &y, const std::string & file_path)
{
    // Create the file stream
    std::ofstream file2write;
    file2write.open(file_path);

    // Easy print
    //file2write << "x" << std::endl << x << std::endl;
    file2write << "y = sin(x)" << std::endl << y;

    // TODO: Try to print coef, exponents (and order) in a organized way so we can print this using Taylor Expansions
    // TODO: later in python?
    // for (int i = 0; i < y.getMaxOrder())

    // Close the stream
    file2write.close();
}