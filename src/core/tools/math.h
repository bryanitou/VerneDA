// Astrodynamics powered by Differential Algebra.
// Copyright (C) 2023 Alberto Fossa'
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU Affero General Public License as
// published by the Free Software Foundation, either version 3 of the
// License, or (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Affero General Public License for more details.
//
// You should have received a copy of the GNU Affero General Public License
// along with this program.  If not, see <https://www.gnu.org/licenses/>.


#pragma once

// System libraries
#include <vector>
#include <stdexcept>
#include <valarray>

namespace tools::math {
    /**
     * Range function.
     * @param a start (included)
     * @param b stop (excluded)
     * @param s step
     * @return range
     */
    std::vector<int> range(int a, int b, int s);


    /**
     * Returns n samples equally spaced in [a,b].
     * @tparam T type of elements
     * @param a left limit
     * @param b right limit
     * @param n number of samples
     * @return samples
     */
    template<typename T>
    std::vector<T> linspace(T a, T b, int n);


    /**
     * Returns a tuple of meshes over (x,y).
     * @tparam T type of elements
     * @param x first array
     * @param y second array
     * @return tuple of meshes
     */
    template<typename T>
    std::tuple<std::vector<std::vector<T>>, std::vector<std::vector<T>>>
    meshgrid(const std::vector<T> &x, const std::vector<T> &y);


    /**
     * Enumerates the 2^n vertices of the n-dimensional hypercube.
     * @tparam T type of elements
     * @param left coordinate of left vertex
     * @param right coordinate of right vertex
     * @param ndim hyperspace dimension
     * @return hypercube vertices
     */
    template<typename T>
    std::vector<std::vector<T>> hypercubeVertices(const T &left, const T &right, int ndim);


    /**
     * Uniform sampling of the edges of the n-dimensional hypercube.
     * @tparam T type of elements
     * @param left coordinate of left vertex
     * @param right coordinate of right vertex
     * @param ndim hyperspace dimension
     * @param ns number of samples per edge
     * @return generated samples
     */
    template<typename T>
    std::vector<std::vector<T>> hypercubeEdges(const T &left, const T &right, int ndim, int ns,
                                               const std::vector<int> &sweep = {},
                                               std::vector<bool> path = {});


    /**
     * Uniform sampling of the edges of the n-dimensional hypercube in [-1,1]^n.
     * @param ndim hyperspace dimension
     * @param ns number of samples per edge
     * @return generated samples
     */
    std::vector<std::vector<double>>
    hypercubeEdges(int ndim, int ns, const std::vector<int> &sweep = {}, const std::vector<bool> &path = {});

    template<typename T>
    std::vector<std::vector<T>>
    hypercubeEdges2(const T &left, const T &right, const int ndim, const int ns, const std::vector<int> &sweep,
                    std::vector<bool> path);

    /**
     * Returns -1 for negative numbers and +1 for positive numbers.
     * @tparam T [in] [T]
     * @param val [in] value
     * @return -1 or 1
     */
    template<typename T>
    int sgn(T val);
}


// Include templates
#include "math_temp.cpp"