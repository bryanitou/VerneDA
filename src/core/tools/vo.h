/**
 * Namespace dedicated to tools.
 */

#pragma once

// System libraries
#include <string>
#include <vector>


namespace tools::vector
{
    /**
     * Vector of integers to string.
     * @param v [in] [std::vector<int>]
     * @param separator [in] [std::string]
     * @return std::string
     */
    std::string int2string(std::vector<int> &v, const std::string& separator = ",");

    /**
     * Vector of integers to string.
     * @param v [in] [std::vector<unsigned int>]
     * @param separator [in] [std::string]
     * @return std::string
     */
    std::string int2string(std::vector<unsigned int> &v, const std::string& separator = ",");
}
