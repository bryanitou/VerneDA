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

#include "tools/math.h"
#include <cmath>
#include "tools/vo.h"

std::vector<int> tools::math::range(const int a, const int b, const int s) {
    std::vector<int> r((b - a) / s);
    for (int i = 0; i < r.size(); i++) {
        r[i] = a + i;
    }
    return r;
}

std::vector<std::vector<double>> tools::math::hypercubeEdges(const int ndim, const int ns, const std::vector<int>& sweep, const std::vector<bool>& path) {
    return tools::math::hypercubeEdges2(-1.0, 1.0, ndim, ns, sweep, path);
}

// Returns value of Binomial Coefficient C(n, k)
// Copied code: https://www.geeksforgeeks.org/binomial-coefficient-dp-9/
int tools::math::binomialCoeff(int n, int k)
{
    // Base Cases
    if (k > n)
        return 0;
    if (k == 0 || k == n)
        return 1;

    // Recur
    return tools::math::binomialCoeff(n - 1, k - 1)
           + tools::math::binomialCoeff(n - 1, k);
}

// Code: https://www.javatpoint.com/factorial-program-in-cpp
int tools::math::factorial(int n)
{
    if (n < 0)
    {
        return -1; /*Wrong value*/
    }
    if (n == 0)
    {
        return 1;  /*Terminating condition*/
    }
    else {
        return (n * tools::math::factorial(n - 1));
    }
}

std::vector<std::vector<unsigned int>> tools::math::get_DA_basis(int n_var, int n_ord)
{
    std::vector<std::vector<unsigned int>> basisVectors;
    std::vector<unsigned int> currentVector(n_var, 0);

    while (true) {
        if (std::accumulate(currentVector.begin(), currentVector.end(), 0) <= n_ord) {
            basisVectors.push_back(currentVector);
        }

        // Find the rightmost non-zero entry
        int j = n_var - 1;
        while (j >= 0 && currentVector[j] == n_ord) {
            currentVector[j] = 0;
            --j;
        }

        if (j < 0) {
            // All entries are at their maximum n_ord, break the loop
            break;
        }

        // Increment the rightmost non-zero entry
        ++currentVector[j];
    }

    return basisVectors;
}