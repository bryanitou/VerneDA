/**
 * Base point class.
 */

#pragma once

// System libraries
#include <vector>
#include <cstdio>
#include <string>

// Project libraries
#include "base/enums.h"

class point {

public: // Constructor

    /**
     * Default constructor.
     */
    point(double x, double y, double z, const std::string &id = "");

    /**
     * Default destructor.
     */
    ~point() = default;

private:// Basic point

    // Point itself
    std::vector<double> p = {0.0, 0.0, 0.0};

    // Point id "if any"
    std::string id{};

private:
    /**
     * Set point
     * @param x [in] [double]
     * @param y [in] [double]
     * @param z [in] [double]
     */
    void set_point(double x, double y, double z);

public:

    /**
     * Modify point
     * @param x
     * @param y
     * @param z
     */
    void clear_point(double x, double y, double z);

    /**
    * Modify point
    * @param x
    * @param y
    * @param z
    */
    void update_point(double x, double y, double z);

    /**
     * Get coordinates of the point
     * @return
     */
    std::vector<double> get_coordinates();

    /**
     * Overload operator to return a parameter: x, y or z.
     * @param position
     * @return
     */
    double operator >(POSITION position);

    /**
     * Overload operator for equality.
     * @param position
     * @return
     */
    bool operator ==(point* point1);
};

