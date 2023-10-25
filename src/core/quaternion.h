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

// DACE libraries
#include "dace/dace.h"

// Project libraries
#include "tools/str.h"

namespace quaternion {

    /**
     * Set quaternion from Euler angles.
     * @param roll [in] [T]
     * @param pitch [in] [T]
     * @param yaw [in] [T]
     * @return quaternion
     */
    template <typename T> std::vector<T> euler2quaternion_temp(T roll, T pitch, T yaw);
    std::vector<double> euler2quaternion(double roll, double pitch, double yaw);
    std::vector<DACE::DA> euler2quaternion(const DACE::DA& roll, const DACE::DA& pitch, const DACE::DA& yaw);

    /**
     * Set quaternion form xyz Gaussian sampled
     * @param x [in] [double]
     * @param y [in] [double]
     * @param z [in] [double]
     * @return quaternion
     */
    std::vector<double> euler2quaternion_fromGaussian(double x, double y, double z);

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

    void scale(DACE::AlgebraicVector<DACE::DA> *q, double num);

    void check_norm(DACE::AlgebraicVector<DACE::DA> *q);

    template <typename T> std::vector<T> quaternion2euler_DACE(T w, T x, T y, T z);
    template <typename T> std::vector<T> quaternion2euler_NORMAL(T w, T x, T y, T z);

    std::vector<double> quaternion2euler(std::vector<double> q);

    std::vector<double> q8_normal_01(int seed);

    double *r8vec_uniform_01_new(int n, int &seed);
};


// Include templates implementation
#include "quaternion_temp.cpp"
