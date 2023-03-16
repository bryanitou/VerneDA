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
#include <vector>

namespace quaternion {

    /**
     * Set quaternion from Euler angles.
     * @param roll [in] [double]
     * @param pitch [in] [double]
     * @param yaw [in] [double]
     */
    std::vector<double> euler2quaternion(double roll, double pitch, double yaw);

     /**
      * Get the norm of the quaterion.
      * @param q
      * @return
      */
    double getnorm(std::vector<double> q);

    std::vector<double> scale(std::vector<double> q, double num);

    /**
 * Get the inverse of the internal quaternion
 */
    std::vector<double>  inverse();

    /**
     * Get inverse from external quaternion.
    * @param q2inv
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
    std::vector<double>  inverse(std::vector<double>  q2inv);


    std::vector<double> q8_multiply(std::vector<double> q1, std::vector<double> q2);
};
