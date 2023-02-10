/**
 * Namespace where enums conversion will be found
 */
#pragma once

// System libraries
#include <string>

// Project libraries
#include "base/enums.h"

namespace enums_conversion
{
    /**
     * Parse VELOCITY enum to string.
     * @param velocity
     * @return
     */
    std::string VELOCITY2str(VELOCITY velocity);

    /**
     * Parse POSITION enum to string.
     * @param position
     * @return
     */
    std::string POSITION2str(POSITION position);
};
