/**
 *
 */

#pragma once

// Include system libraries
# include <string>
# include <utility>

// Include dace
# include "dace/dace.h"

// MATLAB stuff
#include "mex.hpp"

// Project libraries
#include "tools/str.h"

namespace mex_aux
{
    std::vector<double>
    convertMatlabTypedArray2NormalVector(const matlab::data::TypedArray<double> &array2convert);

    double convertMatlabDouble2NormalDouble(const matlab::data::TypedArray<double> &double2convert);

    int convertMatlabInt2NormalInt(const matlab::data::TypedArray<int_least16_t> &int2convert);

    std::string
    convertMatlabStr2NormalStr(const matlab::data::TypedArray<matlab::data::MATLABString> &str2convert);

    template <typename T>
    std::string toUTF8(const  std::basic_string<T,  std::char_traits<T>,  std::allocator<T>>& source);

    template <typename T>
    void fromUTF8(const std::string& source, std::basic_string<T,std::char_traits<T>, std::allocator<T>>& result);
}

// Include template
#include "mex_aux_temp.cpp"
