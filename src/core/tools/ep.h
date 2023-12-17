/**
 * EP: Enum Parser space. Namespace dedicated to tools.
 */

#pragma once

// System libraries
#include <fstream>
#include <filesystem>

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

    /**
     * Parse STATE CONTROL VECTOR to VELOCITY enum.
     * @param state
     * @return VELOCITY enum
     */
    VELOCITY STATE2VELOCITY(STATE state);

    /**
    * Parse STATE CONTROL VECTOR to POSITION enum.
    * @param state
    * @return POSITION enum
    */
    POSITION STATE2POSITION(STATE state);

    std::string PROBLEM2str(PROBLEM problem);

    std::string INTEGRATOR2str(INTEGRATOR integrator);

    std::string ALGORITHM2str(ALGORITHM algorithm);
};
