/**
 * STR: String space. Namespace dedicated to tools.
 */

#pragma once

// System libraries
#include <string>
#include <vector>
#include <cstdarg>
#include <memory>
#include <algorithm>


namespace tools::string
{
    // Auxiliary general values
    namespace keys
    {
        const static std::string bars = "\\";
        const static std::string comma = "\"";
    }

    /**
     * Print to string.
     * @param fmt_str
     * @param ...
     * @return std::string
     */
    std::string print2string(const std::string &fmt_str, ...);

    /**
     * Clean bars and commas.
     * @param str2clean
     * @return std::string
     */
    std::string clean_bars(const std::string &str2clean);
};
