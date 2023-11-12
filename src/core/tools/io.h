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
#include "tools/io_dace.h"

namespace tools::io::dace
{
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
    * Dump evaluated deltas.
    * @param delta [in] [delta]
    * @param file_path [in] [std::filesystem::path]
    */
    void dump_splitting_history(delta* delta, const std::filesystem::path &file_path);

    /**
     * Print all the evolution (evolution of manifolds)
     * @param delta [in] [delta*]
     * @param dir_path [in] [std::filesystem::path]
     */
    void print_manifold_evolution(delta* delta, const std::filesystem::path &dir_path, EVAL_TYPE eval_type);
}
