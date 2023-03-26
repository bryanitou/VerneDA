/**
 * VO: Vector Operations space. Namespace dedicated to tools.
 */

#pragma once

// System libraries
#include <string>
#include <vector>


namespace tools::vector
{
    /**
     * Vector of integers to string.
     * @tparam T [in] [template]
     * @param v [in] [std::vector<T>]
     * @param separator [in] [std::string]
     * @return std::string
     */
     template<typename T>
    std::string num2string(std::vector<T> v, const std::string& separator = ",");
}

// Include templates implementation
#include "vo_temp.cpp"
