/**
 *
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


matlab::data::TypedArray<matlab::data::MATLABString> mex_aux::convertNormalStr2MatlabStr(std::vector<std::string> &c_plus_plus_string, matlabFactory* factory_ptr) {

    auto result = factory_ptr->createArray<matlab::data::MATLABString>({1, c_plus_plus_string.size()});
    std::u16string u16_String{};
    for (unsigned int i = 0; i < c_plus_plus_string.size(); i++)
    {
        fromUTF8(c_plus_plus_string[i], u16_String);

        result[i] = u16_String;
    }


    // Return result
    return result;
}


double *mex_aux::convertMatlab3x3Array2Normal3x3Array(const matlab::data::TypedArray<double> &arr2convert, matlabSharedPtr matlab_ptr_, matlabFactory* factory_ptr_) {
    double *result = (double *) malloc(sizeof(double) * 3 * 3);

    for (int i = 0; i < 3; i ++) {
        for (int j = 0; j < 3; j ++) {
            result[i * 3 + j] = arr2convert[i][j];

            // Prepare string
            auto err2print = tools::string::print2string("i: %d, j: %d, i*3 + j: %d --> %f\n", i, j, i * 3 + j,
                                                         result[i * 3 + j]);

            // Show error
            matlab_ptr_->feval(u"fprintf",
                                     0,
                                     std::vector<matlab::data::Array>(
                                     {factory_ptr_->createScalar(err2print)}));
        }
    }
    // Return result
    return result;
}


matlab::data::TypedArray<double> mex_aux::convertNormalVector2MatlabTypedArray(const dace_array &final_state, matlabFactory* factory_ptr_) {
    // Set result
    matlab::data::TypedArray<double> result = factory_ptr_->createArray<double>(
            {final_state[0].size(), final_state.size()});

    // Iterate and assign
    for (int i = 0; i < final_state.size(); i ++) {
        for (int j = 0; j < final_state[j].size(); j ++) {
            result[j][i] = final_state[i][j];
        }
    }

    // Return result
    return result;
}