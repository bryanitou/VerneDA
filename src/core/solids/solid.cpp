/**
 * Cylinder class.
 */

#include "solid.h"

void solid::addpoint(double x, double y, double z, const std::string& id)
{
    // Check if point is already there
    point new_p(x, y, z, id);

    // Auxiliary bool
    bool same = false;

    // Check if they are the same
    for (auto & p : this->points)
    {
        if (p.operator==(&new_p))
        {
            // Info
            std::printf("Not adding point '%s' since it is already in the list.", id.c_str());

            // Set bool
            same = true;

            // Break loop, nothing else to look for here
            break;
        }
    }

    if (!same)
    {
        // Create a point object and add it
        this->addpoint(new_p);
    }
}

void solid::addpoint(const point& point)
{
    this->points.push_back(point);
}
