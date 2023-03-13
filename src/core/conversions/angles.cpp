/**
 * AT: Angles transformations. Namespace dedicated to tools.
 */

#include "angles.h"

double conversions::angles::deg2rad(double deg)
{
    // Check amount of laps and substract
    int laps = std::ceil(deg / 360);
    deg = deg - laps * 360;

    // Check overlapping and normalize
    while (deg >= 360.0)
    {
        deg -= 360.0;
    }
    while (deg < 0.0)
    {
        deg += 360.0;
    }

    // Return result
    return (deg / 180.0) * M_PI;
}

double conversions::angles::rad2deg(double rad)
{
    // Check amount of laps and substract
    int laps = std::ceil(rad / M_2_PI);
    rad = rad - laps * M_2_PI;

    // Check overlapping and normalize
    while (rad >= M_2_PI)
    {
        rad -= M_2_PI;
    }
    while (rad < 0.0)
    {
        rad += M_2_PI;
    }

    // Return result
    return (rad / M_PI) * 180.0;
}