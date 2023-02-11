/**
 *
 */

#include "ep.h"


[[maybe_unused]] std::string tools::enums::VELOCITY2str(VELOCITY velocity)
{
    // Value to be returned
    std::string result;

    // Fill the value...
    result =
            VELOCITY::X == velocity ? "X" :
            VELOCITY::Y == velocity ? "Y" :
            VELOCITY::Z == velocity ? "Z" : "UNK";

    // Check returned value
    if (result == "UNK")
    {
        printf("WARNING: Could not parse VELOCITY enum. Returning '%s'", result.c_str());
    }

    // Return found value
    return result;
}

[[maybe_unused]] std::string tools::enums::POSITION2str(POSITION position)
{
    // Value to be returned
    std::string result;

    // Fill the value...
    result =
            POSITION::X == position ? "X" :
            POSITION::Y == position ? "Y" :
            POSITION::Z == position ? "Z" : "UNK";

    // Check returned value
    if (result == "UNK")
    {
        printf("WARNING: Could not parse POSITION enum. Returning '%s'", result.c_str());
    }

    // Return found value
    return result;

}

std::string tools::enums::DISTRIBUTION2str(DISTRIBUTION distribution)
{
    // Value to be returned
    std::string result;

    // Fill the value...
    result =
            DISTRIBUTION::GAUSSIAN == distribution ? "GAUSSIAN" :
            DISTRIBUTION::UNIFORM == distribution ? "UNIFORM" : "UNK";

    // Check returned value
    if (result == "UNK")
    {
        printf("WARNING: Could not parse DISTRIBUTION enum. Returning '%s'", result.c_str());
    }

    // Return found value
    return result;

}