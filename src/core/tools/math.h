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
#include <numeric>

#include "dace/dace.h"
#include "dace/AlgebraicMatrix.h"

namespace tools::math {
    /**
     * Range function.
     * @param a start (included)
     * @param b stop (excluded)
     * @param s step
     * @return range
     */
    std::vector<int> range(int a, int b, int s);


    int binomialCoeff(int n, int k);
    int factorial(int n);

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


    /**+
     * Get the DA basis for this algebra
     * @param n_var
     * @param n_ord
     * @return
     */
    std::vector<std::vector<unsigned int>> get_DA_basis(int n_var, int n_ord);

    /**
     * Convert from r6 to spherical
     * @tparam T
     * @param r6
     * @return
     */
    template <typename T> DACE::AlgebraicVector<T> get_spherical(DACE::AlgebraicVector<T> r6);

    /**
     * Kronocker product
     * @tparam T
     * @param A
     * @param B
     * @return
     */
    template <typename T>
    DACE::AlgebraicMatrix<T> Kroneckerproduct_vect(DACE::AlgebraicVector<T>& A, DACE::AlgebraicVector<T>& B);
    template <typename T>
    DACE::AlgebraicMatrix<T> Kroneckerproduct(DACE::AlgebraicMatrix<T>A, DACE::AlgebraicMatrix<T>B);
    template <typename T>
    DACE::AlgebraicMatrix<T> vector2matrix(DACE::AlgebraicVector<T> v);
    template<typename T>
    DACE::AlgebraicMatrix<T> vectorize2d21d(DACE::AlgebraicMatrix<T> v);
}



// Include templates
#include "math_temp.cpp"