/**
 * EP: Enum Parser space. Namespace dedicated to tools.
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
        printf("WARNING: Could not parse DISTRIBUTION enum. Returning '%s'\n", result.c_str());
    }

    // Return found value
    return result;

}

std::string tools::enums::INTEGRATOR2str(INTEGRATOR integrator)
{
    // Value to be returned
    std::string result;

    // Fill the value...
    result =
            INTEGRATOR::RK4 == integrator ? "RK4" :
            INTEGRATOR::STATIC == integrator ? "STATIC" :
            INTEGRATOR::EULER == integrator ? "EULER" :
            INTEGRATOR::RK78 == integrator ? "RK78" :
            INTEGRATOR::NA == integrator ? "NA" : "UNK";

    // Check returned value
    if (result == "UNK")
    {
        printf("WARNING: Could not parse INTEGRATOR enum. Returning '%s'\n", result.c_str());
    }

    // Return found value
    return result;
}

std::string tools::enums::PROBLEM2str(PROBLEM problem)
{
    // Value to be returned
    std::string result;

    // Fill the value...
    result =
            PROBLEM::TWO_BODY == problem ? "TWO_BODY" :
            PROBLEM::FREE_TORQUE_MOTION == problem ? "FREE_TORQUE_MOTION" :
            PROBLEM::POL2CART == problem ? "POL2CART" :
            PROBLEM::FREE_FALL_OBJECT == problem ? "FREE_FALL_OBJECT" :
            PROBLEM::NA == problem ? "NA" : "UNK";

    // Check returned value
    if (result == "UNK")
    {
        printf("WARNING: Could not parse PROBLEM enum. Returning '%s'\n", result.c_str());
    }

    // Return found value
    return result;

}

std::string tools::enums::ALGORITHM2str(ALGORITHM algorithm)
{
    // Value to be returned
    std::string result;

    // Fill the value...
    result =
            ALGORITHM::LOADS    == algorithm ? "LOADS" :
            ALGORITHM::ADS      == algorithm ? "ADS" :
            ALGORITHM::NONE     == algorithm ? "NONE" :
            ALGORITHM::NA       == algorithm ? "NA" : "UNK";

    // Check returned value
    if (result == "UNK")
    {
        printf("WARNING: Could not parse ALGORITHM enum. Returning '%s'\n", result.c_str());
    }

    // Return found value
    return result;

}

POSITION tools::enums::STATE2POSITION(STATE state)
{
    auto result =
            state == STATE::PX ? POSITION::X :
            state == STATE::PY ? POSITION::Y :
            state == STATE::PZ ? POSITION::Z : POSITION::NA;

    return result;
}

VELOCITY tools::enums::STATE2VELOCITY(STATE state)
{
    auto result =
            state == STATE::VX ? VELOCITY::X :
            state == STATE::VY ? VELOCITY::Y :
            state == STATE::VZ ? VELOCITY::Z : VELOCITY::NA;

    return result;
}