/**
 * Cylinder class.
 */

#pragma once

// System libraries
#include <vector>


class cylinder {

public:

    /**
     * Default constructor.
     */
    cylinder() = default;

    /**
     * Default destructor.
     */
    ~cylinder() = default;

private:

    std::vector<double> upper_center = {0.0, 0.0, 0.0};
    std::vector<double> lower_center = {0.0, 0.0, 0.0};

private:


};
