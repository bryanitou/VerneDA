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
