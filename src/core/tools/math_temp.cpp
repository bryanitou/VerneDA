/**
 * Namespace dedicated to tools. -> Templates place
 */


template<typename T>
std::vector<T> tools::math::linspace(T a, T b, int n) {
    if (n < 2) {
        throw std::invalid_argument("number of elements must be greater than one");
    } else if (n == 2) {
        return std::vector<T>({a, b});
    } else {
        std::vector<T> r(n);
        T h = (b - a) / (n - 1);
        for (unsigned int i = 0; i < (n - 1); i++) r[i] = a + h * i;
        r[n - 1] = b;
        return r;
    }
}

template<typename T>
std::tuple<std::vector<std::vector<T>>, std::vector<std::vector<T>>>
tools::math::meshgrid(const std::vector<T> &x, const std::vector<T> &y) {
    std::vector<std::vector<T>> xm(y.size());
    std::vector<std::vector<T>> ym(y.size());
    for (unsigned int i = 0; i < y.size(); i++) {
        xm[i] = std::vector<T>(x);
        ym[i] = std::vector<T>(x.size());
        for (unsigned int j = 0; j < x.size(); j++) ym[i][j] = y[i];
    }
    return std::tuple(xm, ym);
}

template<typename T>
std::vector<std::vector<T>> tools::math::hypercubeVertices(const T &left, const T &right, const int ndim) {
    std::vector<std::vector<T>> out(std::pow(2, ndim));
    for (unsigned int i = 0; i < out.size(); i++)
    {
        std::vector<T> row(ndim);
        for (unsigned int j = 0; j < ndim; j++)
        {
            row[j] = (i & (1 << j)) == 0 ? right : left;
        }
        out[i] = row;
    }
    return out;
}

template<typename T>
std::vector<std::vector<T>> tools::math::hypercubeEdges(const T &left, const T &right, const int ndim, const int ns,
                                                        const std::vector<int>& sweep, std::vector<bool> path)
{
    // Count sweeps
    unsigned int n_sweeps = 0;
    if (sweep.empty()) { n_sweeps = ndim; }
    else { for (auto & sw : sweep){n_sweeps += sw;} }
    int edg2sw = (int) (ns*std::pow(2, n_sweeps));

    // Prepare path in case of emptiness
    if(path.empty()) {for (int d = 0; d < edg2sw; d++) {path.push_back(true); } }

    // single edge
    std::vector<T> edg_dir = tools::math::linspace(left, right, ns);
    std::vector<T> edg_rev = edg_dir; std::reverse(edg_rev.begin(), edg_rev.end());

    // vertices
    std::vector<std::vector<T>> vrt = tools::math::hypercubeVertices(left, right, ndim - 1);

    // all edges
    unsigned int i, j, k, l, m, p, n{0};
    std::vector<std::vector<T>> out(n_sweeps * edg_dir.size() * vrt.size());
    for (i = 0; i < out.size(); i++) {
        out[i] = std::vector<T>(ndim);
    }
    for (i = 0; i < vrt.size(); i++) {
        for (j = 0; j < ndim; j++) {
            p = n >= edg2sw ? n % edg2sw == 0 ? 0 : p : p;
            l = 0;
            if (!(sweep[j])) {
                continue;
            }
            for (k = 0; k < ndim; k++) {
                if (j == k) {
                    for (m = 0; m < ns; m++) {
                        out[n + m][k] = path[j] ? edg_dir[m] : edg_rev[m];
                    }
                    p++;
                }
                else {
                    for (m = 0; m < ns; m++) {
                        out[n + m][k] = vrt[i][l];
                    }
                    l++;
                }
            }
            n += ns;
        }
    }
    // TODO: Debug
    // int h = 0;
    // // printing vector..
    // for (auto & o : out)
    // {
    //     std::cout << std::to_string(h) << " : " << tools::vector::num2string(o) << std::endl;
    //     h++;
    // }

    return out;
}

template<typename T>
std::vector<std::vector<T>> tools::math::hypercubeEdges2(const T &left, const T &right, const int ndim, const int ns,
                                                        const std::vector<int>& sweep, std::vector<bool> path)
{
    // Count sweeps
    unsigned int n_sweeps = 0;
    if (sweep.empty()) { n_sweeps = 1; }
    else { for (auto & sw : sweep){n_sweeps += sw;} }
    int edg2sw = (int) (ns*std::pow(2, n_sweeps));

    // Prepare path in case of emptiness
    if(path.empty()) {for (int d = 0; d < edg2sw; d++) {path.push_back(true); } }

    // single edge
    std::vector<T> edg_dir = tools::math::linspace(left, right, ns);
    std::vector<T> edg_rev = edg_dir; std::reverse(edg_rev.begin(), edg_rev.end());

    // Compute degree of freedom per point
    auto dof = ndim - n_sweeps;

    // vertices
    std::vector<std::vector<T>> vrt = tools::math::hypercubeVertices(left, right, dof);

    // Compute number of lines to draw
    int n_lines = (int) std::pow(2, n_sweeps);

    // Compute output vector
    std::vector<std::vector<T>> out(n_lines * edg_dir.size() * vrt.size());

    // Initialize array
    for (int i = 0; i < out.size(); i++) {
        out[i] = std::vector<T>(ndim);
    }

    // Some indexes
    int n_vrt = vrt.size();
    int n_edg = edg_dir.size();

    //

    int p{}, t{}, f, m;
    // Iterate through every line
    for (int i = 0; i < n_lines; i++)
    {
        // Iterate through the dots in the line
        for (int j = 0; j < n_edg; j++)
        {
            // Iterate through all the possible bounds
            for (int g = 0; g < n_vrt; g++)
            {
                // Compute position and reset iterator
                f = i * n_edg * n_vrt + j*n_vrt + g;
                m = 0;

                // Iterate through the dimensions
                for (int d = 0; d < ndim; d++)
                {
                    if (sweep[d] && d == t)
                    {
                        out[f][d] = path[p] ? edg_dir[j] : edg_rev[j];
                    }
                    else if (sweep[d])
                    {
                        out[f][d] = p > 1 ? -1 : 1;
                    }
                    else
                    {
                        out[f][d] =vrt[g][m];
                        m++;
                    }
                }
            }
        }

        // Update t index
        t = t == (n_sweeps - 1) ? 0 : t + 1;

        // Update path index
        p++;
    }

    // for (int i = 0; i < out.size(); i++) {
    //     std::fprintf(stdout, "%s\n", tools::vector::num2string(out[i]).c_str());
    // }

    return out;
}


template <typename T> int tools::math::sgn(T val)
{
    return (T(0) < val) - (val < T(0));
}


template <typename T> DACE::AlgebraicVector<T> tools::math::get_spherical(DACE::AlgebraicVector<T> r6)
{
    DACE::AlgebraicVector<T> spherical_coord(3);
    auto x = r6[0];
    auto y = r6[1];
    auto z = r6[2];
    spherical_coord[0] = DACE::sqrt( x*x + y*y + z*z); // Range
    spherical_coord[1] = DACE::atan2( y , x); // Azimuth
    spherical_coord[2] = DACE::atan2( z , DACE::sqrt(x*x + y*y)); // Elevation

    return spherical_coord;
}

template <typename T>
DACE::AlgebraicMatrix<T> tools::math::Kroneckerproduct_vect(
        DACE::AlgebraicVector<T>& A, DACE::AlgebraicVector<T>& B)
{
    // Convert to matrix Nx1
    auto A_matrix = tools::math::vector2matrix(A); // Vector of 1 column
    auto B_matrix = tools::math::vector2matrix(B).transpose(); // Vector of 1 column

    // Print matrix
    for (int i = 0; i < A_matrix.nrows(); i++)
    {
        for (int j = 0; j < A_matrix.ncols(); j++)
        {
            std::fprintf(stdout, "Matrix_A[%2d, %2d] = \n", i, j);
            std::fprintf(stdout, "%s \n",A_matrix.at(i, j).toString().c_str());
        }
    }
    std::fprintf(stdout, "Matrix print finished\n");
    return tools::math::Kroneckerproduct(A_matrix, B_matrix);
}

template <typename T>
DACE::AlgebraicMatrix<T> tools::math::vector2matrix(DACE::AlgebraicVector<T> v)
{
    // Assign values
    unsigned int nrows = v.size();
    DACE::AlgebraicMatrix<T> result(nrows, 1);
    // std::fprintf(stdout, "About to convert to matrix, the vector: %s\n", v.toString().c_str());

    for (int i = 0; i < nrows; i++)
    {
        // Set column vector
        result.at(i, 0) = v[i];
    }

    return result;
}

template <typename T>
DACE::AlgebraicMatrix<T> tools::math::vectorize2d21d(DACE::AlgebraicMatrix<T> v)
{
    // Assign values
    unsigned int n_col = 1;
    DACE::AlgebraicMatrix<T> result(v.nrows() * v.ncols(), n_col);

    for (int i = 0; i < v.nrows(); i++)
    {
        // Set column vector
        for (int j = 0; j < v.ncols(); j++)
        {
            result.at(i * v.nrows() + j, 0) = v.at(i, j);
        }
    }

    return result;
}

template <typename T>
DACE::AlgebraicMatrix<T> tools::math::Kroneckerproduct(
        DACE::AlgebraicMatrix<T>  A, DACE::AlgebraicMatrix<T>  B)
{
    // Auxiliary variables
    unsigned int rowa = A.nrows();
    unsigned int rowb = B.nrows();
    unsigned int cola = A.ncols();
    unsigned int colb = B.ncols();
    unsigned int idx_row;
    unsigned int idx_col;

    DACE::AlgebraicMatrix<T>  result(rowa * rowb, cola * colb);
    std::fprintf(stdout, "Kronocker product will have size: (%2d, %2d)\n", result.nrows(), result.ncols());
    // i loops till rowa
    for (int i = 0; i < rowa; i++) {
        for (int k = 0; k < cola; k++) {
            for (int j = 0; j < rowb; j++) {
                for (int l = 0; l < colb; l++) {
                    idx_row = i * rowb + j;
                    idx_col = k * colb + l;
                    std::fprintf(stdout, "Kronecker product: R(%2d, %2d) = A(%d, %d) * B(%d, %d)\n",
                                 idx_row, idx_col, i, k, j, l);
                    result.at(idx_row, idx_col) = A.at(i, k) * B.at(j, l);
                }
            }
        }
    }

    return result;
}