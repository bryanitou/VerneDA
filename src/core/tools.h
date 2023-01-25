/**
 * Namespace dedicated to tools.
 */

#pragma once

// System libraries
#include <fstream>

// DA library
#include "dace/dace.h"

namespace tools {


    /**
     *
     * @param y
     * @param file_path
     */
    void dump_variables(DACE::DA &y, const std::string &file_path);
};
