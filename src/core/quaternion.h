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
    std::vector<double> q = {0.0, 0.0, 0.0, 0.0};

public: // Private functions

    /**
     * Set quaternion from Euler angles.
     * @param roll [in] [double]
     * @param pitch [in] [double]
     * @param yaw [in] [double]
     */
    static std::vector<double> get_from_Euler(double roll, double pitch, double yaw);

private:
    /**
     * Set quaternion as is.
     * @param q_new [in] [double]
     */
    void set_quaternion(std::vector<double> q_new);

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
     static std::vector<double>  inverse(std::vector<double>  q2inv);

    /**
     * Rotate quaternion given an axis.
     * @param q_axis
     * @return
     */
    std::vector<double> rotate(std::vector<double>  q_axis);

     /**
      * Rotate quaternion given an axis in Euler angles.
      * @param roll [in] [double]
      * @param pitch [in] [double]
      * @param yaw [in] [double]
      * @return quaternion
      */
     std::vector<double>  rotate(double roll, double pitch, double yaw);
};
