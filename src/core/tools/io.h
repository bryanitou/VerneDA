/**
 * IO: In / Out space. Namespace dedicated to tools.
 */

#pragma once

// System libraries
#include <fstream>
#include <filesystem>
#include <unordered_map>

// Project libraries
#include "delta.h"

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
        void dump_variables(DACE::DA &y, DACE::DA &x, const std::string &func_form,
                            const std::string &var_form, const std::filesystem::path &file_path);

        /**
         * Dump Algebraic Vector type
         * @param DA_v [in] [DACE::AlgebraicVector<DACE::DA>]
         * @param file_path [in] [std::filesystem::path]
         */
        void dump_algebraic_vector(const DACE::AlgebraicVector<DACE::DA>& DA_v, const std::filesystem::path &file_path);

        /**
         * Dump evaluated deltas.
         * @param delta [in] [delta]
         * @param file_path [in] [std::filesystem::path]
         */
        void dump_eval_deltas(delta* delta, const std::filesystem::path &file_path, EVAL_TYPE eval_type = EVAL_TYPE::FINAL_DELTA);

        /**
         * Dump non evaluated deltas.
         * @param delta [in] [delta]
         * @param file_path [in] [std::filesystem::path]
         */
        void dump_non_eval_deltas(delta* delta, const std::filesystem::path &file_path);

        /**
         * Function used by other functions here within this same scope. Prints monomials in a stream.
         * @param file2write [in] [std::ofstream]
         * @param da_var [in] [DACE::DA]
         * @param n_da_var [in] [bool]
         * @param monomial_masked [in] [bool]
         * @param d [in] [int]
         * @param v [in] [int]
         */
        void print_each_monomial(std::ofstream& file2write, const DACE::DA &da_var, bool n_da_var, bool monomial_masked,
                                 std::vector<int> idx, EVAL_TYPE eval_type = EVAL_TYPE::FINAL_DELTA);

        /**
        * Dump evaluated deltas.
        * @param delta [in] [delta]
        * @param file_path [in] [std::filesystem::path]
        */
        void dump_splitting_history(delta* delta, const std::filesystem::path &file_path);

        /**
         * Print each evaluated sample
         * @param deltas_poly [in] [std::vector<DACE::AlgebraicVector<double>>]
         * @param file2write [in] [std::ofstream]
         * @param eval_type [in] [EVAL_TYPE]
         */
        void print_each_delta(std::vector<DACE::AlgebraicVector<double>> deltas_poly, std::ofstream &file2write,
                              EVAL_TYPE eval_type, bool print_header = true);

        /**
         * Print each patch wall
         * @param patches [in] [std::vector<std::vector<DACE::AlgebraicVector<double>>>]
         * @param file2write [in] [std::ofstream]
         * @param eval_type [in] [EVAL_TYPE]
         * @param print_header [in] [bool]
         */
        void print_each_patch_wall(std::vector<std::vector<DACE::AlgebraicVector<double>>> patches,
                                   std::ofstream &file2write, EVAL_TYPE eval_type, bool print_header = true);

        /**
         * Print all the evolution (evolution of manifolds)
         * @param delta [in] [delta*]
         * @param dir_path [in] [std::filesystem::path]
         */
        void print_manifold_evolution(delta* delta, const std::filesystem::path &dir_path, EVAL_TYPE eval_type);
    }

    /**
     * Calls to python file, passes the output file and executes it.
     * @param python_executable [in] [std::string]
     * @param args [in] [td::unordered_map<std::string, std::string>]
     * @param async [in] [bool]
     */
    void plot_variables(const std::string &python_executable,
                        const std::unordered_map<std::string, std::string> &args, bool async = false);

    /**
     * Calls to python file, passes the output file and executes it.
     * @param args_str [in] [std::string]
     * @param async [in] [bool]
     */
    void make_film(std::string args_str, bool async);
};
