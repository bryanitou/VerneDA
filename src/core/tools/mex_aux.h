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

// Some definitions
# define dace_array std::vector<DACE::AlgebraicVector<double>>
# define matlabSharedPtr std::shared_ptr<matlab::engine::MATLABEngine>
# define matlabFactory matlab::data::ArrayFactory

class mex_aux
{
public:
    mex_aux() = default;

    ~mex_aux() = default;

private:// Private

    // Matlab pointers
    // std::shared_ptr<matlab::engine::MATLABEngine> matlab_ptr_;
    // volatile matlab::data::ArrayFactory* factory_ptr_;

public:
    // Set pointers
    // void set_matlab_ptr(std::shared_ptr<matlab::engine::MATLABEngine>& matlab_ptr)
    // {
    //     this->matlab_ptr_ = matlab_ptr;
    // }
    // void set_factory_ptr(matlab::data::ArrayFactory* factory_ptr)
    // {
    //     this->factory_ptr_ = factory_ptr;
    // }

    // Getters
    // auto get_matlab_ptr_() {return matlab_ptr_; }
    // auto get_factory_ptr_() {return factory_ptr_; }

    // Static
    static std::vector<double> convertMatlabTypedArray2NormalVector(const matlab::data::TypedArray<double> &array2convert);

    static double convertMatlabDouble2NormalDouble(const matlab::data::TypedArray<double> &double2convert);

    static int convertMatlabInt2NormalInt(const matlab::data::TypedArray<int_least16_t> &int2convert);

    static std::string convertMatlabStr2NormalStr(const matlab::data::TypedArray<matlab::data::MATLABString> &str2convert);

    // Dynamic
    matlab::data::TypedArray<matlab::data::MATLABString> convertNormalStr2MatlabStr(std::vector<std::string> &c_plus_plus_string, matlabFactory* factory_ptr);

    double *convertMatlab3x3Array2Normal3x3Array(const matlab::data::TypedArray<double> &arr2convert, matlabSharedPtr matlab_ptr_, matlabFactory* factory_ptr_);

    matlab::data::TypedArray<double>
    convertNormalVector2MatlabTypedArray(const std::vector<DACE::AlgebraicVector<double>> &final_state, matlabFactory* factory_ptr_);

    // Templates
    template<typename T>
    std::string toUTF8(const std::basic_string<T, std::char_traits<T>, std::allocator<T>> &source);

    template<typename T>
    void fromUTF8(const std::string &source, std::basic_string<T, std::char_traits<T>, std::allocator<T>> &result);
};

// Include template
#include "mex_aux_temp.cpp"
