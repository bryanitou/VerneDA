/**
 * Namespace where enums conversion will be found
 */

#include "enums_conversion.h"


std::string enums_conversion::VELOCITY2str(VELOCITY velocity)
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

std::string enums_conversion::POSITION2str(POSITION position)
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