/**
 * VO: Vector Operations space. Namespace dedicated to tools.
 */

#pragma once

// System libraries
#include <string>
#include <vector>
#include <typeinfo>

// Include 3rd party library
#include "dace/dace.h"

// Include tools
#include "tools/str.h"

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
     std::string num2string(std::vector<T> v, const std::string& separator = ",",
                            const std::string& precision = "", bool close = true);

    /**
     * Vector of integers to string.
     * @param v [in] [DACE::AlgebraicVector<DACE::DA>]
     * @param separator [in] [std::string]
     * @return std::string
     */
     std::string da_cons2string(const DACE::AlgebraicVector<DACE::DA>& v, const std::string& separator = ",",
                                const std::string& precision = "", bool close = true);
}

// Include templates implementation
#include "vo_temp.cpp"
