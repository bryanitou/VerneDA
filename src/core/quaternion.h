/**
 * Quaternions class.
 * @details:
 *  - A nice web to visualize quaternions: https://quaternions.online/
 *  - A nice video to understand them from 3Blue1Brown: https://www.youtube.com/watch?v=zjMuIxRvygQ
 *  - To get inspired from some code: https://people.sc.fsu.edu/~jburkardt/cpp_src/quaternions/quaternions.html
 */

#pragma once

// System libraries
#include <cmath>

class quaternion {

public: // Constructors

    /**
     * Default constructor.
     */
    quaternion(double roll, double pitch, double yaw);

    /**
     * Default constructor.
     */
    quaternion() = default;

    /**
     * Default destructor.
     */
    ~quaternion() = default;

private: // Attributes

    // Quaternion definition: q = a + b*i + c*j + d*k
    double q[4]{};

private: // Private functions

    /**
     * Set quaternion from Euler angles.
     * @param roll [in] [double]
     * @param pitch [in] [double]
     * @param yaw [in] [double]
     */
    static double * get_from_Euler(double roll, double pitch, double yaw);

    void set_quaternion(double* q);

    /**
     * Get the inverse of the quaternion q
     * @param q
     * @details
     *
     *    A quaternion is a quadruplet (A,B,C,D) of real numbers, which
     *    may be written as
     *
     *      Q = A + Bi + Cj + Dk.
     *
     *    The inverse of Q is
     *
     *      inverse ( Q ) = conjugate ( Q ) / ( norm ( Q ) )^2.
     * @return
     */
     double* inverse();
     static double* inverse(const double* q);


     double* rotate(const double* q_axis);
     double* rotate(double roll, double pitch, double yaw);
};
