/**
 * Cylinder class.
 */

#pragma once

// System libraries
#include <vector>

// Project libraries
#include "geometry/point.h"


class solid
{
public:

    /**
     * Default constructor.
     */
    solid() = default;

    /**
     * Default destructor.
     */
    ~solid() = default;

private:

    // A vector of points
    std::vector<point> points{};

    // Identifier
    std::string id{};

public:

    /**
     * Add point to the solid
     * @param x [in] [double]
     * @param y [in] [double]
     * @param z [in] [double]
     * @param id [in] [std::string]
     */
    void addpoint(double x, double y, double z, const std::string& id = "");



    /**
     * Add point
     * @param point [in] [point]
     */
    void addpoint(const point& point);


    /**
     * Add points vector
     * @param point_list [in] [point]
     */
    void addpoints(const std::vector<point>& point_list);
};
