/**
 * Namespace dedicated to tools.
 */

#pragma once

// System libraries
#include <fstream>
#include <filesystem>

// DA library
#include "dace/dace.h"

namespace tools {


    /**
     * Dumps the images in a given file path
     * @param y images.
     * @param func_form name of the function form (i.e. 'y = sin(x)').
     * @param file_path file path.
     */
    void dump_variables(DACE::DA &y, const std::string &func_form, const std::filesystem::path &file_path);

    /**
     * Calls to python file, passes the output file and executes it.
     * @param path2file path to the output '.txt' file.
     * @param python_executable path to the python script.
     * @param async asynchronous task.
     * @param silent silent task.
     */
    void plot_variables(std::filesystem::path &path2file, const std::string &python_executable,
                        bool async, bool silent = true);
};
