/**
 * Namespace dedicated to tools.
 */

#pragma once

// System libraries
#include <string>
#include <vector>
#include <cstdarg>
#include <memory>


namespace tools::string
{
    /**
     * Print to string.
     * @param fmt_str
     * @param ...
     * @return std::string
     */
    std::string print2string(const std::string &fmt_str, ...);
};
