/**
 * Namespace dedicated to tools.
 */

#pragma once

// System libraries
#include <fstream>
#include <filesystem>

// DA library
#include "dace/dace.h"

// Project libraries
#include "base/enums.h"

namespace tools {

    namespace io
    {
        /**
     * Dumps the images in a given file path
     * @param y DA images.
     * @param x DA component.
     * @param func_form name of the function form (i.e. 'y = sin(x)').
     * @param func_form variable form, point and derivative.
     * @param file_path file path.
     */
        [[maybe_unused]] void dump_variables(DACE::DA &y, DACE::DA &x, const std::string &func_form,const std::string &var_form, const std::filesystem::path &file_path);

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
    }

    namespace enums
    {
        /**
         * Parse VELOCITY enum to string.
         * @param velocity
         * @return
         */
        [[maybe_unused]] std::string VELOCITY2str(VELOCITY velocity);

        /**
         * Parse POSITION enum to string.
         * @param position
         * @return
         */
        [[maybe_unused]] std::string POSITION2str(POSITION position);

        /**
         * Parse DISTRIBUTION enum to string.
         * @param distribution
         * @return
         */
        std::string DISTRIBUTION2str(DISTRIBUTION distribution);
    }

};
