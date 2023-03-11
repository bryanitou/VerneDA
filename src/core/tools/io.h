/**
 * IO: In / Out space. Namespace dedicated to tools.
 */

#pragma once

// System libraries
#include <fstream>
#include <filesystem>

// DA library
#include "dace/dace.h"
#include "delta.h"

// Project libraries
#include "base/enums.h"

// Imported tools
#include "tools/vo.h"
#include "tools/str.h"

namespace tools::io
{
    namespace dace
    {
        /**
        * Dumps the images in a given file path
        * @param y DA images.
        * @param x DA component.
        * @param func_form name of the function form (i.e. 'y = sin(x)').
        * @param func_form variable form, point and derivative.
        * @param file_path file path.
        */
        [[maybe_unused]] void dump_variables(DACE::DA &y, DACE::DA &x, const std::string &func_form,
                                             const std::string &var_form, const std::filesystem::path &file_path);

        void dump_variables(DACE::DA &func, const std::string& id, const std::filesystem::path &file_path);

        void dump_algebraic_vector(const DACE::AlgebraicVector<DACE::DA>& DA_v, const std::filesystem::path &file_path);

        void dump_deltas(delta* delta, const std::filesystem::path &file_path);
    }

    /**
     * Calls to python file, passes the output file and executes it.
     * @param path2file path to the output '.txt' file.
     * @param python_executable path to the python script.
     * @param span read python file to understand this argument.
     * @param async asynchronous task.
     * @param silent silent task.
     */
    [[maybe_unused]] void plot_variables(std::filesystem::path &path2file, const std::string &python_executable, int span = 1,
                                         bool async = false, bool silent = true);
};
