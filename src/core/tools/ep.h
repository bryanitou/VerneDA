/**
 * Namespace dedicated to tools.
 */

#pragma once

// System libraries
#include <fstream>
#include <filesystem>

// DA library
#include "delta.h"

// Project libraries
#include "base/enums.h"

namespace tools::enums
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

        VELOCITY STATE2VELOCITY(STATE state);
        POSITION STATE2POSITION(STATE state);
    };
