/**
 * Cylinder class.
 */

#include "solid.h"

void solid::addpoint(double x, double y, double z, const std::string& point_id)
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
            std::printf("Not adding point '%s' since it is already in the list.", point_id.c_str());

            // Set bool
            same = true;

            // Break loop, nothing else to look for here
            break;
        }
    }

    if (!same)
    {
        // Info
        std::printf("Adding point '%s' to solid '%s'", point_id.c_str(), this->id.c_str());

        // Create a point object and add it
        this->addpoint(new_p);
    }
}

void solid::addpoints(const std::vector<point> &point_list)
{
    // Add points looping through them
    for (auto &new_p: point_list)
    {
        this->addpoint(new_p);
    }
}

void solid::addpoint(const point& point)
{
    this->points.push_back(point);
}
