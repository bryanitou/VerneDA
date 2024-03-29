/**
 * MEX Auxiliary static functions
 */

#include "mex_aux.h"


std::vector<double> mex_aux::convertMatlabTypedArray2NormalVector(const matlab::data::TypedArray<double> &array2convert) {
    // Set result
    // TODO: std::vector<double> result(array2convert.begin(), array2convert.end());
    std::vector<double> result;

    // Pass to vector
    for (auto &in: array2convert) {
        result.push_back(in);
    }

    // Return result
    return result;
}

double mex_aux::convertMatlabDouble2NormalDouble(const matlab::data::TypedArray<double> &double2convert) {
    // Return result
    return double2convert[0];
}

int mex_aux::convertMatlabInt2NormalInt(const matlab::data::TypedArray<int_least16_t> &int2convert) {
    // Return result
    return int2convert[0];
}

std::string mex_aux::convertMatlabStr2NormalStr(const matlab::data::TypedArray<matlab::data::MATLABString> &str2convert) {
    // Return result
    return std::string(str2convert[0]);
}

std::vector<std::string> mex_aux::convertMatlabStrVector2NormalStrVector(const matlab::data::TypedArray<matlab::data::MATLABString> &str2convert) {
    auto dim = str2convert.getDimensions();
    std::fprintf(stdout, "hi %zu\n", dim[0]);
    std::fprintf(stdout, "hi %zu\n", dim[1]);

    std::vector<std::string> result(dim[0]);

    for (unsigned int i = 0; i < result.size(); i++) {result[i] = std::string(str2convert[i][0]);
        std::fprintf(stdout, "%s\n",result[i].c_str());}
    return result;
}

/*
void mex_aux::checkArguments(matlab::mex::ArgumentList outputs, matlab::mex::ArgumentList inputs, MEX_FILE_TYPE type)
{
    // Auxiliary variable
    std::string err2print;
    bool print_error = false;

    // Inputs to check?
    auto inputs2check = type == MEX_FILE_TYPE::GET_DA ? mex_aux::inputs_MEX_GET_DA() :
            type == MEX_FILE_TYPE::VSAOD ? mex_aux::inputs_MEX_VSAOD()  : mex_aux::inputs();

    // Check arguments
    for (auto & i : inputs2check.vectors2check6)
    {
        // Safety check
        if (i >= inputs.size())
        {
            print_error = true;
        }

        if (!print_error)
        {
            if (inputs[i].getType() != matlab::data::ArrayType::DOUBLE ||
                inputs[i].getType() == matlab::data::ArrayType::COMPLEX_DOUBLE ||
                (inputs[i].getNumberOfElements() != 6 && inputs[i].getNumberOfElements() != 7)) {
                print_error = true;
            }
        }

        if (print_error)
        {
            // String to show
            err2print = tools::string::print2string("Input %d must be an array of 6 doubles", i + 1);

            // Show string
            std::fprintf(stderr, "ERROR: %s\n", err2print.c_str());
        }
    }

    // Check arguments
    for (auto & i : inputs2check.vectors2check3)
    {
        // Safety check
        if (i >= inputs.size())
        {
            print_error = true;
        }

        if (!print_error)
        {
            if (inputs[i].getType() != matlab::data::ArrayType::DOUBLE ||
                inputs[i].getType() == matlab::data::ArrayType::COMPLEX_DOUBLE ||
                inputs[i].getNumberOfElements() != 3) {
                print_error = true;
            }
        }

        if (print_error)
        {
            // String to show
            err2print = tools::string::print2string("Input %d must be an array of 3 doubles", i + 1);

            // Show string
            std::fprintf(stderr, "ERROR: %s\n", err2print.c_str());
        }
    }

    // Check arguments
    for (auto & i : inputs2check.doubles2check)
    {
        // Safety check
        if (i >= inputs.size())
        {
            print_error = true;
        }

        if (!print_error)
        {
            if (inputs[i].getType() != matlab::data::ArrayType::DOUBLE ||
                inputs[i].getType() == matlab::data::ArrayType::COMPLEX_DOUBLE ||
                inputs[i].getNumberOfElements() != 1) {
                print_error = true;
            }
        }


        if (print_error)
        {
            // Prepare string
            err2print = tools::string::print2string("Input %d must be of type double", i + 1);

            // Show string
            std::fprintf(stderr, "ERROR: %s\n", err2print.c_str());

            // Reset print error
            print_error = false;
        }
    }

    // Check arguments
    for (auto & i : inputs2check.int2check)
    {
        // Safety check
        if (i >= inputs.size())
        {
            print_error = true;
        }

        if (!print_error)
        {
            if (inputs[i].getType() != matlab::data::ArrayType::INT16 ||
                inputs[i].getType() == matlab::data::ArrayType::COMPLEX_INT16 ||
                inputs[i].getNumberOfElements() != 1) {
                print_error = true;
            }
        }


        if (print_error)
        {
            // Prepare string
            err2print = tools::string::print2string("Input %d must be of type int16", i + 1);

            // Show error
            std::fprintf(stderr, "ERROR: %s\n", err2print.c_str());

            // Reset print error
            print_error = false;
        }
    }

    // Check arguments
    for (auto & i : inputs2check.str2check)
    {
        // Safety check
        if (i >= inputs.size())
        {
            print_error = true;
        }

        if (!print_error)
        {
            if (inputs[i].getType() != matlab::data::ArrayType::MATLAB_STRING ||
                inputs[i].getNumberOfElements() != 1) {
                print_error = true;
            }
        }


        if (print_error)
        {
            // Prepare string
            err2print = tools::string::print2string("Input %d must be of type MATLAB_STRING", i + 1);

            // Show error
            std::fprintf(stderr, "ERROR: %s\n", err2print.c_str());

            // Reset print error
            print_error = false;
        }
    }

    // Check number of outputs
    if (outputs.size() > 1) {
        std::fprintf(stderr, "ERROR: Only one output is returned\n");
    }
}
*/