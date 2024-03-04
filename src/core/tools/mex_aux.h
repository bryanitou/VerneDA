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
#include "base/enums.h"

namespace mex_aux
{
    class inputs {
    public:
        inputs() = default;
        ~inputs() = default;

        // Define vectors to check the inputs
        [[maybe_unused]] std::vector<int> vectors2check6 = {};
        std::vector<int> vectors2check3 = {};
        std::vector<int> doubles2check = {};
        std::vector<int> int2check = {};
        std::vector<int> str2check = {};
    };

    class inputs_MEX_VSAOD : public inputs {
    public:
        inputs_MEX_VSAOD(){init(); };
        ~inputs_MEX_VSAOD() = default;

        void init()
        {
            // Define vectors to check the inputs
            this->vectors2check6 = {0, 1};
            this->vectors2check3 = {2};
            this->doubles2check = {3, 4};
            this->int2check = {5, 6};
            this->str2check = {7};
        }
    };
    class inputs_MEX_GET_DA : public inputs {
    public:
        inputs_MEX_GET_DA() {init();};
        ~inputs_MEX_GET_DA() = default;

        void init()
        {
            // Define vectors to check the inputs
            this->vectors2check6 = {0, 1};
            this->doubles2check = {2};
        }
    };

    std::vector<double>
    convertMatlabTypedArray2NormalVector(const matlab::data::TypedArray<double> &array2convert);

    double convertMatlabDouble2NormalDouble(const matlab::data::TypedArray<double> &double2convert);

    int convertMatlab16Int2NormalInt(const matlab::data::TypedArray<int_least16_t> &int2convert);
    int convertMatlab32Int2NormalInt(const matlab::data::TypedArray<int_least32_t> &int2convert);

    std::string
    convertMatlabStr2NormalStr(const matlab::data::TypedArray<matlab::data::MATLABString> &str2convert);

    template <typename T>
    std::string toUTF8(const  std::basic_string<T,  std::char_traits<T>,  std::allocator<T>>& source);

    template <typename T>
    void fromUTF8(const std::string& source, std::basic_string<T,std::char_traits<T>, std::allocator<T>>& result);

    // void checkArguments(matlab::mex::ArgumentList outputs, matlab::mex::ArgumentList inputs, MEX_FILE_TYPE type);
    std::vector<std::string>
    convertMatlabStrVector2NormalStrVector(const matlab::data::TypedArray<matlab::data::MATLABString> &str2convert);

    std::vector<std::vector<std::string>>
    convertMatlabStrArray2NormalStrVector(const matlab::data::TypedArray<matlab::data::MATLABString> &str2convert);
}

// Include template
#include "mex_aux_temp.cpp"
