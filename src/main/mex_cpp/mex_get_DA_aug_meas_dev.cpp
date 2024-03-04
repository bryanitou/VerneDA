/**
 * Propagate DA vector for given amount of time
 */
// MEX thingy
#include "mex.hpp"
#include "mexAdapter.hpp"

// MEX aux library
#include "tools/mex_aux.h"

// DACE libraries
#include "dace/dace.h"

// Project libraries
#include "delta.h"
#include "tools/vo.h"

class MexFunction : public matlab::mex::Function {

private:
    // Get pointer to engine
    std::shared_ptr<matlab::engine::MATLABEngine> matlabPtr = getEngine();

    // Get array factory
    std::shared_ptr<matlab::data::ArrayFactory> factoryPtr;
public:

    void operator()(matlab::mex::ArgumentList outputs, matlab::mex::ArgumentList inputs) override {
        try {
            // Set factory
            this->factoryPtr = std::make_shared<matlab::data::ArrayFactory>();

            // TODO: Check the health of the passed inputs
            // mex_aux::checkArguments(outputs, inputs, MEX_FILE_TYPE::GET_DA);

            // Extract inputs -----------
            auto predicted_state = mex_aux::convertMatlabStrVector2NormalStrVector(inputs[0]);
            auto sensor_noise = mex_aux::convertMatlabStrVector2NormalStrVector(inputs[1]);
            auto second_moments = mex_aux::convertMatlabTypedArray2NormalVector(inputs[2]);

            // Number of variables
            int n_var = (int) predicted_state.size();

            // Initialize DACE with 6 variables
            DACE::DA::init(2, n_var);

            // Convert state to DA vector
            auto predicted_state_DA = DACE::AlgebraicVector<DACE::DA>(n_var);
            for (int i = 0; i < n_var; i++) {
                predicted_state_DA[i] = DACE::DA::fromString(predicted_state[i]);
            }
            auto sensor_noise_DA = DACE::AlgebraicVector<DACE::DA>(n_var);
            for (int i = 0; i < n_var; i++) {
                sensor_noise_DA[i] = DACE::DA::fromString(sensor_noise[i]);
            }
            auto second_moments_DA = DACE::AlgebraicVector<DACE::DA>(n_var);
            for (int i = 0; i < n_var; i++) {
                second_moments_DA[i] = second_moments[i];
            }
            fprintf(stdout, "Algebra: \n");
            fprintf(stdout, "Order: %2d\n", DACE::DA::getMaxOrder());
            fprintf(stdout, "Variables: %2d\n", DACE::DA::getMaxVariables());

            // Perform logic here -----------
            // Now we can propagate
            auto y = (predicted_state_DA + sensor_noise_DA).eval(second_moments_DA);
            DACE::AlgebraicVector<DACE::DA> dy = predicted_state_DA - y + sensor_noise_DA;
            fprintf(stdout, "y size: %2d\n", y.size());
            fprintf(stdout, "y: \n");
            fprintf(stdout, "y: %s\n",y.toString().c_str());

            fprintf(stdout, "dy size: %2d\n", dy.size());
            fprintf(stdout, "dy: %s\n", dy.toString().c_str());
            fprintf(stdout, "dy: %s\n", dy.toString().c_str());

            // Make kronocker product
            auto dy2 = tools::math::Kroneckerproduct_vect(dy, dy);
            std::fprintf(stdout, "dy2 size: (%2d, %2d)\n", dy2.nrows(), dy2.ncols());
            std::fprintf(stdout, "dy2: %s\n", matrix2str(dy2).c_str());

            // Augmented measurement deviation vector
            auto P2dydy = get_P2dydy(second_moments, dy);
            fprintf(stdout, "P2dydy size: (%2d, %2d)\n", P2dydy.nrows(), P2dydy.ncols());
            // DACE::DA P3dydydy[dim3];
            // DACE::DA P4dydydydy[dim4];
            // get_P3dydydy(prev_second_moments, dy, n_var, P3dydydy);
            // get_P4dydydydy(prev_second_moments, dy, n_var, P4dydydydy);
            auto P3dydydy_new = get_P3dydydy_2(second_moments, dy, n_var);
            fprintf(stdout, "P3dydydy size: (%2d, %2d)\n", P3dydydy_new.nrows(), P3dydydy_new.ncols());
            auto P4dydydydy_new = get_P4dydydydy_2(second_moments, dy, n_var);
            fprintf(stdout, "P4dydydydy size: (%2d, %2d)\n", P4dydydydy_new.nrows(), P4dydydydy_new.ncols());
            auto vect_P2dydy = tools::math::vectorize2d21d(P2dydy);
            fprintf(stdout, "vect(P2dydy) size: (%2d, %2d)\n", vect_P2dydy.nrows(), vect_P2dydy.ncols());
            // DACE::DA P2dy2dy2[dim4];
            // get_P2dy2dy2(vect_P2dydy, n_var, P4dydydydy, P2dy2dy2);
            auto P2dy2dy2_new = get_P2dy2dy2_2(n_var, vect_P2dydy, P4dydydydy_new);
            fprintf(stdout, "P2dy2dy2 size: (%2d, %2d)\n", P2dy2dy2_new.nrows(), P2dy2dy2_new.ncols());
            auto P2dydy2_new = P3dydydy_new.transpose();
            fprintf(stdout, "P2dydy2 size: (%2d, %2d)\n", P2dydy2_new.nrows(), P2dydy2_new.ncols());
            auto P2dy2dy_new = P3dydydy_new;
            fprintf(stdout, "P2dy2dy size: (%2d, %2d)\n", P2dy2dy_new.nrows(), P2dy2dy_new.ncols());
            // *----------------* Augmented measurement covariance matrix  *----------------*
            auto P2dYdY = get_PdYdY_2(n_var, P2dydy, P2dydy2_new, P2dy2dy_new, P2dy2dy2_new);
            fprintf(stdout, "P2dYdY size: (%2d, %2d)\n", P2dYdY.nrows(), P2dYdY.ncols());
            auto P2dYdY_aug = (1e20 * P2dYdY);
            std::fprintf(stdout, "P2dYdY: %s\n", matrix2str(P2dYdY_aug).c_str());

            std::fprintf(stdout, "det(P2dYdY): %s\n", P2dYdY_aug.det().toString().c_str());

            auto P2dYdY_inv = P2dYdY_aug.inv() * 1e-20;
            fprintf(stdout, "inv(P2dYdY) size: (%2d, %2d)\n", P2dYdY_inv.nrows(), P2dYdY_inv.ncols());

            // *----------------* The augmented state-measurement cross covariance matrix  *----------------*

            // DACE::DA P2dy2dy[dim3];
            // transpose3d(n_var, P2dydy2_new, P2dy2dy);

            // DACE::AlgebraicMatrix<DACE::DA> rest = dy2 - vect_P2dydy;
            // Get the deviation in x
            DACE::AlgebraicVector<DACE::DA> dx = predicted_state_DA  - predicted_state_DA.cons();
            auto Pxdy = get_P2dxdy(n_var, dx, dy, second_moments);
            fprintf(stdout, "Pxdy size: (%2d, %2d)\n", Pxdy.nrows(), Pxdy.ncols());
            auto Pxdydy = get_P2xdy2(n_var, dx, dy, second_moments);
            fprintf(stdout, "Pxdydy size: (%2d, %2d)\n", Pxdydy.nrows(), Pxdydy.ncols());
            auto PxdY = get_PxdY(n_var, Pxdy, Pxdydy);
            fprintf(stdout, "PxdY size: (%2d, %2d)\n", PxdY.nrows(), PxdY.ncols());
            DACE::AlgebraicMatrix<DACE::DA> Kalman_gain = PxdY * P2dYdY_inv;
            fprintf(stdout, "Kalman matrix size: (%2d, %2d)\n", Kalman_gain.nrows(), Kalman_gain.ncols());

            // Return DA vector as str
            outputs[0] = inputs[0];// convertNormalVector2MatlabTypedArray(dy);
        }
        catch (int) {
            std::fprintf(stdout, "ERROR: Something unexpected happened...");
        }
    }

    std::string matrix2str(DACE::AlgebraicMatrix<DACE::DA> m)
    {
        std::string row_str{};
        for (int i = 0; i < m.nrows(); i++)
        {
            for (int j = 0; j < m.ncols(); j++)
            {
                row_str += tools::string::print2string("(%2d, %d):\n %s", i, j, m.at(i, j).toString().c_str());
            }

            row_str += "\n";

        }
        return row_str;
    }


    matlab::data::TypedArray<double>
    convertNormalVector2MatlabTypedArray(const std::vector<double> &v) {
        // Set result
        matlab::data::TypedArray<double> result = this->factoryPtr->createArray<double>(
                {v.size()});

        // Iterate and assign
        for (int i = 0; i < v.size(); i++)
        {
            result[i] = v[i];
        }

        // Return result
        return result;
    }

    static DACE::AlgebraicMatrix<DACE::DA> get_P2dydy(std::vector<double>& prev_2nd_moment, const DACE::AlgebraicVector<DACE::DA>& dy)
    {
        // Get the current workframe of the algebra
        auto n_var = (int) dy.size();

        // Generate result vector
        DACE::AlgebraicMatrix<DACE::DA> result(n_var);

        // Iterate through all the variables
        for (int i = 0; i < n_var; i++)
        {
            for (int j = 0; j < n_var; j++)
            {
                result.at(i, j) = (dy[i]*dy[j]).eval(prev_2nd_moment);
            }
        }

        // Done!
        return result;
    }

    static void get_P3dydydy(std::vector<double>& prev_2nd_moment, const DACE::AlgebraicVector<DACE::DA>& dy,
                             unsigned int n_var, DACE::DA res[])
    {
        for (int i = 0; i < n_var; i++)
        {
            for (int j = 0; j < n_var; j++)
            {
                for (int k = 0; k < n_var; k++)
                {
                    res[i*n_var*n_var + j*n_var + k]= (dy[i] * dy[j] * dy[k]).eval(prev_2nd_moment);
                }
            }
        }
    }

    static DACE::AlgebraicMatrix<DACE::DA> get_P3dydydy_2(std::vector<double>& prev_2nd_moment, const DACE::AlgebraicVector<DACE::DA>& dy,
                             unsigned int n_var)
    {
        DACE::AlgebraicMatrix<DACE::DA> result(n_var*n_var, n_var);
        for (int i = 0; i < n_var; i++)
        {
            for (int j = 0; j < n_var; j++)
            {
                for (int k = 0; k < n_var; k++)
                {
                    result.at(i*n_var + j, k) = (dy[i] * dy[j] * dy[k]).eval(prev_2nd_moment);
                }
            }
        }
        return result;
    }

    static void get_P4dydydydy(std::vector<double>& prev_2nd_moment, const DACE::AlgebraicVector<DACE::DA>& dy,
                               unsigned int n_var, DACE::DA res[])
    {
        for (int i = 0; i < n_var; i++)
        {
            for (int j = 0; j < n_var; j++)
            {
                for (int k = 0; k < n_var; k++)
                {
                    for (int l = 0; l < n_var; l++)
                    {
                        res[i*n_var*n_var*n_var + j*n_var*n_var +
                        k*n_var + l]= (dy[i] * dy[j] * dy[k] * dy[l]).eval(prev_2nd_moment);
                    }
                }
            }
        }
    }

    static DACE::AlgebraicMatrix<DACE::DA> get_P4dydydydy_2(std::vector<double>& prev_2nd_moment, const DACE::AlgebraicVector<DACE::DA>& dy,
                               unsigned int n_var)
    {
        DACE::AlgebraicMatrix<DACE::DA> result(n_var*n_var, n_var*n_var);
        for (int i = 0; i < n_var; i++)
        {
            for (int j = 0; j < n_var; j++)
            {
                for (int k = 0; k < n_var; k++)
                {
                    for (int l = 0; l < n_var; l++)
                    {
                        result.at(i*n_var + j, k*n_var + l) = (dy[i] * dy[j] * dy[k] * dy[l]).eval(prev_2nd_moment);
                    }
                }
            }
        }
        return result;
    }

    static void get_P2dy2dy2(const DACE::AlgebraicMatrix<DACE::DA>& vect_P2dydy, unsigned int n_var,
                             DACE::DA P4dydydydy[], DACE::DA res[])
    {
        // Auxiliary
        unsigned int idx;
        // Vectorize matrix
        DACE::AlgebraicMatrix<DACE::DA> product = vect_P2dydy * vect_P2dydy.transpose();
        for (int i = 0; i < n_var; i++)
        {
            for (int j = 0; j < n_var; j++)
            {
                for (int k = 0; k < n_var; k++)
                {
                    for (int l = 0; l < n_var; l++)
                    {
                        idx = i*n_var*n_var*n_var + j*n_var*n_var + k*n_var + l;
                        res[idx] = P4dydydydy[idx] - product.at(n_var * i + j, n_var * k + l);
                    }
                }
            }
        }
    }

    static DACE::AlgebraicMatrix<DACE::DA> get_P2dy2dy2_2(unsigned int n_var,
                                                        const DACE::AlgebraicMatrix<DACE::DA>& vect_P2dydy,
                                                        const DACE::AlgebraicMatrix<DACE::DA>& P4dydydydy)
    {
        // Auxiliary
        unsigned int idx1;
        unsigned int idx2;
        DACE::AlgebraicMatrix<DACE::DA> result(n_var*n_var, n_var*n_var);

        // Vectorize matrix
        DACE::AlgebraicMatrix<DACE::DA> product = vect_P2dydy * vect_P2dydy.transpose(); // NxN

        for (int i = 0; i < n_var; i++)
        {
            for (int j = 0; j < n_var; j++)
            {
                for (int k = 0; k < n_var; k++)
                {
                    for (int l = 0; l < n_var; l++)
                    {
                        idx1 = n_var * i + j;
                        idx2 = n_var * k + l;
                        result.at(idx1, idx2) = P4dydydydy.at(idx1, idx2) - product.at(idx1, idx2);
                    }
                }
            }
        }
        return result;
    }

    static void transpose3d(unsigned int n_var, DACE::DA dim3[], DACE::DA res[])
    {
        unsigned int idx1;
        unsigned int idx2;
        for (int i = 0; i < n_var; i++)
        {
            for (int j = 0; j < n_var; j++)
            {
                for (int k = 0; k < n_var; k++)
                {
                    idx1 = i*n_var*n_var + j*n_var + k;
                    idx2 = i*n_var*n_var + k*n_var + j;
                    res[idx1] = dim3[idx2];
                }
            }
        }
    }

    static void get_PdYdY(unsigned int n_var, DACE::DA dim3[], DACE::DA res[])
    {
        unsigned int idx1;
        unsigned int idx2;
        for (int i = 0; i < n_var; i++)
        {
            for (int j = 0; j < n_var; j++)
            {
                for (int k = 0; k < n_var; k++)
                {
                    idx1 = i*n_var*n_var + j*n_var + k;
                    idx2 = i*n_var*n_var + k*n_var + j;
                    res[idx1] = dim3[idx2];
                }
            }
        }
    }

    static DACE::AlgebraicMatrix<DACE::DA> get_PdYdY_2(unsigned int n_var,
                            DACE::AlgebraicMatrix<DACE::DA>& P2dydy,
                            DACE::AlgebraicMatrix<DACE::DA>& P2dydy2,
                            DACE::AlgebraicMatrix<DACE::DA>& P2dy2dy,
                            DACE::AlgebraicMatrix<DACE::DA>& P2dy2dy2
                            )
    {
        // Auxiliary
        unsigned int idx_row;
        unsigned int idx_col;

        // Length of the final matrix
        unsigned int nrows = n_var*n_var + n_var;
        unsigned int ncols = n_var*n_var + n_var;

        // Indexes to collect
        unsigned int n_rows11 = P2dydy.nrows();
        unsigned int n_cols11 = P2dydy.ncols();

        DACE::AlgebraicMatrix<DACE::DA> result(nrows, ncols);

        for (unsigned int i = 0; i < nrows; i++)
        {
            for (unsigned int j = 0; j < ncols; j++)
            {
                // Compute the index to collect 'idx_row' and 'idx_row'
                idx_row = i > n_rows11 ? i - n_rows11 : i;
                idx_col = j > n_cols11 ? j - n_cols11 : j;
                result.at(i, j) =
                        i < n_rows11 && j < n_cols11 ? P2dydy.at(idx_row, idx_col) :
                        i < n_rows11 && j > n_cols11 ? P2dydy2.at(idx_row, idx_col) :
                        i > n_rows11 && j < n_cols11 ? P2dy2dy.at(idx_row, idx_col) :
                        P2dy2dy2.at(idx_row, idx_col);
            }
        }

        // Done!
        return result;
    }

    static DACE::AlgebraicMatrix<DACE::DA> get_P2dxdy(unsigned int n_var, DACE::AlgebraicVector<DACE::DA>& dx,
                                                     DACE::AlgebraicVector<DACE::DA>& dy,
                                                     std::vector<double>& prev_moments)
    {
        // Squared matrix
        DACE::AlgebraicMatrix<DACE::DA> result(n_var);

        // Fill the matrix
        for (int i = 0; i < n_var; i++)
        {
            for (int j = 0; j < n_var; j++)
            {
                result.at(i, j) = (dx[i] * dy[j]).eval(prev_moments);
            }
        }

        // Done !
        return result;
    }

    static DACE::AlgebraicMatrix<DACE::DA> get_P2xdy2(unsigned int n_var, DACE::AlgebraicVector<DACE::DA>& dx,
                                                      DACE::AlgebraicVector<DACE::DA>& dy,
                                                      std::vector<double>& prev_moments)
    {
        // Squared matrix
        DACE::AlgebraicMatrix<DACE::DA> result(n_var,  n_var * n_var);

        // Fill the matrix
        for (int i = 0; i < n_var; i++)
        {
            for (int j = 0; j < n_var; j++)
            {
                for (int k = 0; k < n_var; k++)
                {
                    result.at(i, j * n_var + k) = (dx[i] * dy[j] * dy[k]).eval(prev_moments);
                }
            }
        }

        // Done !
        return result;
    }

    static DACE::AlgebraicMatrix<DACE::DA> get_PxdY(unsigned int n_var, DACE::AlgebraicMatrix<DACE::DA>& P2dxdy,
                                                     DACE::AlgebraicMatrix<DACE::DA>& P2dxdy2)
    {
        // Auxiliary
        unsigned int idx_col;

        // Length of the final matrix
        unsigned int nrows = n_var;
        unsigned int ncols = n_var + n_var*n_var;

        // Indexes to collect
        unsigned int n_cols11 = P2dxdy.ncols();

        // Build rectangular matrix
        DACE::AlgebraicMatrix<DACE::DA> result(nrows, ncols);

        for (int i = 0; i < nrows; i++)
        {
            for (int j = 0; j < ncols; j++)
            {
                // Compute the index to collect 'idx_col' and 'idx_col'
                idx_col = j > n_cols11 ? j - (n_cols11 - 1) : i;
                result.at(i, j) = i < n_cols11 ? P2dxdy.at(i, idx_col) : P2dxdy2.at(i, idx_col);
            }
        }

        // Done !
        return result;
    }
};
