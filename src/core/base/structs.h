/**
 * Structs container
 */

#pragma once

// System libraries
#include <string>

// Project libraries
#include "base/enums.h"

namespace structs
{
    namespace output
    {
        // Directories for films
        struct film {
            std::string output_dir_frames{};
            std::string output_dir_source{};
            FILM type{FILM::NA};
        };

        // Output files
        struct wdc_o { // Walls Deltas Centers (+ others)
            std::string output_walls{};
            std::string output_deltas{};
            std::string output_centers{};
            std::string output_prefix{};

            // For attitude
            std::string output_sphere{};
            std::string output_distribution{};
        };
    }

    namespace input
    {

    }

    // struct outputs
    struct out_obj {
        std::vector<output::film> films{};
        std::vector<output::wdc_o> wdcs{};
    };
}

