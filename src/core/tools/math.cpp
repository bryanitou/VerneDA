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