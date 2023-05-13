/**
 * JSON Parser.
 */
#pragma once

// Include system libraries
#include <string>
#include <vector>

// Include JSON thirdparty parser
#include "RSJparser.tcc"

// Include project libraries
#include "tools/str.h"
#include "conversions/angles.h"
#include "specs/specifications.h"

namespace json_parser
{
    /**
     * Parses the input file
     * @param filepath std::string
     */
    specifications parse_input_file(const std::string& filepath);
};

