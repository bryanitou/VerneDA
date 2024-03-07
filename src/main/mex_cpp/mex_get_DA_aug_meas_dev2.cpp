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
            auto pred_meas_dy = mex_aux::convertMatlabStrVector2NormalStrVector(inputs[0]);
            auto second_moments_meas_pred_dy = mex_aux::convertMatlabTypedArray2NormalVectorArray(inputs[1]);
            auto third_moments_meas_pred_dy = mex_aux::convertMatlabTypedArray2NormalVectorArray(inputs[2]);
            auto fourth_moments_meas_pred_dy = mex_aux::convertMatlabTypedArray2NormalVectorArray(inputs[3]);
            // auto sensor_noise = mex_aux::convertMatlabStrVector2NormalStrVector(inputs[1]);
            // auto second_moments_pred_state_x = mex_aux::convertMatlabTypedArray2NormalVectorArray(inputs[2]);
            // auto third_moments_pred_meas_y = mex_aux::convertMatlabTypedArray2NormalVectorArray(inputs[4]);
            // auto fourth_moments_pred_meas_y = mex_aux::convertMatlabTypedArray2NormalVectorArray(inputs[5]);

            // Number of variables
            int n_var_prob = (int) pred_meas_dy.size();

            // Initialize DACE with 6 variables
            DACE::DA::init(2, n_var_prob);

            // Convert state to DA vector
            auto predicted_meas_dy_DA = DACE::AlgebraicVector<DACE::DA>(n_var_prob);
            for (int i = 0; i < n_var_prob; i++) {
                predicted_meas_dy_DA[i] = DACE::DA::fromString(pred_meas_dy[i]);
            }
            // auto sensor_noise_DA = DACE::AlgebraicVector<DACE::DA>(n_var_prob);
            // for (int i = 0; i < n_var_prob; i++) {
            //     sensor_noise_DA[i] = DACE::DA::fromString(sensor_noise[i]);
            // }
            fprintf(stdout, "Algebra: \n");
            fprintf(stdout, "Order: %2d\n", DACE::DA::getMaxOrder());
            fprintf(stdout, "Variables: %2d\n", DACE::DA::getMaxVariables());

            // Make kronocker product
            auto dy2 = tools::math::Kroneckerproduct_vect(predicted_meas_dy_DA, predicted_meas_dy_DA);
            std::fprintf(stdout, "dy2 size: (%2d, %2d)\n", dy2.nrows(), dy2.ncols());
            std::fprintf(stdout, "dy2: %s\n", matrix2str(dy2).c_str());

            // Augmented measurement deviation vector
            //auto P2dydy = get_P2dydy(second_moments_meas_pred_dy, predicted_meas_dy_DA);
            //fprintf(stdout, "P2dydy size: (%2d, %2d)\n", P2dydy.nrows(), P2dydy.ncols());
////
            //auto P3dydydy_new = get_P3dydydy_2(third_moments_meas_pred_dy, predicted_meas_dy_DA);
            //fprintf(stdout, "P3dydydy size: (%2d, %2d)\n", P3dydydy_new.nrows(), P3dydydy_new.ncols());
////
            //auto P4dydydydy_new = get_P4dydydydy_2(fourth_moments_meas_pred_dy, predicted_meas_dy_DA);
            //fprintf(stdout, "P4dydydydy size: (%2d, %2d)\n", P4dydydydy_new.nrows(), P4dydydydy_new.ncols());
//
            // auto vect_P2dydy = tools::math::vectorize2d21d(P2dydy);
            // fprintf(stdout, "vect(P2dydy) size: (%2d, %2d)\n", vect_P2dydy.nrows(), vect_P2dydy.ncols());
            // // DACE::DA P2dy2dy2[dim4];
            // // get_P2dy2dy2(vect_P2dydy, n_var_prob, P4dydydydy, P2dy2dy2);
            // auto P2dy2dy2_new = get_P2dy2dy2_2(n_var_prob, vect_P2dydy, P4dydydydy_new);
            // fprintf(stdout, "P2dy2dy2 size: (%2d, %2d)\n", P2dy2dy2_new.nrows(), P2dy2dy2_new.ncols());
            // auto P2dydy2_new = P3dydydy_new.transpose();
            // fprintf(stdout, "P2dydy2 size: (%2d, %2d)\n", P2dydy2_new.nrows(), P2dydy2_new.ncols());
            // auto P2dy2dy_new = P3dydydy_new;
            // fprintf(stdout, "P2dy2dy size: (%2d, %2d)\n", P2dy2dy_new.nrows(), P2dy2dy_new.ncols());
            // // *----------------* Augmented measurement covariance matrix  *----------------*
            //fprintf(stdout, "second_moments_meas_pred_dy size: (%2d, %2d)\n", second_moments_meas_pred_dy.size(), second_moments_meas_pred_dy[0].size());
            //fprintf(stdout, "third_moments_meas_pred_dy size: (%2d, %2d)\n", third_moments_meas_pred_dy.size(), third_moments_meas_pred_dy[0].size());
            //fprintf(stdout, "fourth_moments_meas_pred_dy size: (%2d, %2d)\n", fourth_moments_meas_pred_dy.size(), fourth_moments_meas_pred_dy[0].size());

            auto dy_2nd_mom = convert2DAmatrix(second_moments_meas_pred_dy);
            auto dy_3rd_mom = convert2DAmatrix(third_moments_meas_pred_dy).transpose();
            auto dy_4th_mom = convert2DAmatrix(fourth_moments_meas_pred_dy);
            fprintf(stdout, "dy_2nd_mom size: (%2d, %2d)\n", dy_2nd_mom.nrows(), dy_2nd_mom.ncols());
            fprintf(stdout, "dy_3rd_mom size: (%2d, %2d)\n", dy_3rd_mom.nrows(), dy_3rd_mom.ncols());
            fprintf(stdout, "dy_4th_mom size: (%2d, %2d)\n", dy_4th_mom.nrows(), dy_4th_mom.ncols());

            auto P2dYdY = get_PdYdY_2(n_var_prob, dy_2nd_mom, dy_3rd_mom, dy_4th_mom);
             fprintf(stdout, "P2dYdY size: (%2d, %2d)\n", P2dYdY.nrows(), P2dYdY.ncols());
             // auto P2dYdY_aug = P2dYdY;
             // std::fprintf(stdout, "P2dYdY: %s\n", matrix2str(P2dYdY_aug).c_str());
             // std::fprintf(stdout, "P2dYdY dump: %s\n", matrix2str_dump(P2dYdY_aug).c_str());
//
            // std::fprintf(stdout, "det(P2dYdY): %s\n", P2dYdY_aug.det().toString().c_str());
//
            // // auto P2dYdY_inv = P2dYdY_aug.inv();
            // fprintf(stdout, "inv(P2dYdY) size: (%2d, %2d)\n", P2dYdY_inv.nrows(), P2dYdY_inv.ncols());

            // *----------------* The augmented state-measurement cross covariance matrix  *----------------*

            // DACE::DA P2dy2dy[dim3];
            // transpose3d(n_var_prob, P2dydy2_new, P2dy2dy);

            // DACE::AlgebraicMatrix<DACE::DA> rest = dy2 - vect_P2dydy;
            // Get the deviation in x
            // DACE::AlgebraicVector<DACE::DA> dx = predicted_meas_dy_DA  - predicted_meas_dy_DA.cons();
            // auto Pxdy = get_P2dxdy(n_var_prob, dx, dy, second_moments_pred_state_x);
            // fprintf(stdout, "Pxdy size: (%2d, %2d)\n", Pxdy.nrows(), Pxdy.ncols());
            // auto Pxdydy = get_P2xdy2(n_var_prob, dx, dy, second_moments);
            // fprintf(stdout, "Pxdydy size: (%2d, %2d)\n", Pxdydy.nrows(), Pxdydy.ncols());
            // auto PxdY = get_PxdY(n_var_prob, Pxdy, Pxdydy);
            // fprintf(stdout, "PxdY size: (%2d, %2d)\n", PxdY.nrows(), PxdY.ncols());
            // DACE::AlgebraicMatrix<DACE::DA> Kalman_gain = PxdY * P2dYdY_inv;
            // fprintf(stdout, "Kalman matrix size: (%2d, %2d)\n", Kalman_gain.nrows(), Kalman_gain.ncols());

            // Return DA vector as str
            outputs[0] = convertNormalVector2MatlabTypedArray(P2dYdY);// convertNormalVector2MatlabTypedArray(dy);
        }
        catch (int) {
            std::fprintf(stdout, "ERROR: Something unexpected happened...");
        }
    }

    DACE::AlgebraicMatrix<double> convert2DAmatrix(std::vector<std::vector<double>> v)
    {
        unsigned int nrows = v.size();
        unsigned int ncols = v[0].size();

        DACE::AlgebraicMatrix<double> result(nrows, ncols);

        for (int i = 0; i < nrows; i++)
        {
            for (int j = 0; j < ncols; j++)
            {
                result.at(i, j) = v[i][j];
            }
        }
        return result;

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

    std::string matrix2str_dump(DACE::AlgebraicMatrix<DACE::DA> m)
    {
        std::string row_str{};
        for (int i = 0; i < m.nrows(); i++)
        {
            for (int j = 0; j < m.ncols() - 1; j++)
            {
                row_str += tools::string::print2string("%.20f, ", i, j, m.at(i, j).cons());
            }
            row_str += tools::string::print2string("%.20f", i,  m.ncols() - 1, m.at(i,  m.ncols() - 1).cons());
            row_str += "\n";

        }
        return row_str;
    }


    matlab::data::TypedArray<double>
    convertNormalVector2MatlabTypedArray(const DACE::AlgebraicMatrix<double> &v) {
        // Set result
        matlab::data::TypedArray<double> result = this->factoryPtr->createArray<double>(
                {v.nrows(), v.ncols()});

        // Iterate and assign
        for (int i = 0; i < v.nrows(); i++)
        {
            for (int j = 0; j < v.ncols(); j++)
            {
                result[i][j] = v.at(i,j);
            }
        }

        // Return result
        return result;
    }

    static DACE::AlgebraicMatrix<DACE::DA> get_P2dydy(std::vector<std::vector<double>>& prev_2nd_moment, const DACE::AlgebraicVector<DACE::DA>& dy)
    {
        // Auxiliary coefficients
        double coeff1;
        double coeff2;

        // Get the current workframe of the algebra
        auto n_var_obs = (int) dy.size();

        // Get the current workframe of the algebra
        auto n_ord_da = (unsigned long) DACE::DA::getMaxOrder();
        auto n_var_da = (unsigned long) DACE::DA::getMaxVariables();
        auto basis = tools::math::get_DA_basis((int) n_var_da, (int) n_ord_da);

        // Generate result vector
        DACE::AlgebraicMatrix<DACE::DA> result(n_var_obs);

        // Iterate through all the variables
        for (int i = 0; i < n_var_obs; i++)
        {
            for (int j = 0; j < n_var_obs; j++)
            {
                for (const auto & basi : basis)
                {
                    // Get the constants for this base
                    coeff1 = dy[i].getCoefficient(basi);
                    coeff2 = dy[j].getCoefficient(basi);
                    result.at(i, j) += coeff1*coeff2;
                }
                result.at(i, j) *= prev_2nd_moment[i][j];
            }
        }

        // Done!
        return result;
    }

    static DACE::AlgebraicMatrix<DACE::DA> get_P3dydydy_2(std::vector<std::vector<double>>& prev_3rd_moment, const DACE::AlgebraicVector<DACE::DA>& dy)
    {
        // Auxiliary coefficients
        double coeff1;
        double coeff2;
        double coeff3;

        // Get num of observables
        auto n_var_obs = (int) dy.size();

        // Get the current work-frame of the algebra
        auto n_ord_da = (unsigned long) DACE::DA::getMaxOrder();
        auto n_var_da = (unsigned long) DACE::DA::getMaxVariables();
        auto basis = tools::math::get_DA_basis((int) n_var_da, (int) n_ord_da);

        // Generate result vector
        DACE::AlgebraicMatrix<DACE::DA> result(n_var_obs);

        for (int i = 0; i < n_var_obs; i++)
        {
            for (int j = 0; j < n_var_obs; j++)
            {
                for (int k = 0; k < n_var_obs; k++)
                {
                    for (const auto & basi : basis)
                    {
                        // Get the constants for this base
                        coeff1 = dy[i].getCoefficient(basi);
                        coeff2 = dy[j].getCoefficient(basi);
                        coeff3 = dy[k].getCoefficient(basi);
                        result.at(i*n_var_obs + j, k) += coeff1*coeff2*coeff3;
                    }

                    result.at(i*n_var_obs + j, k) *= prev_3rd_moment[i*n_var_obs + j][j];
                }
            }
        }
        return result;
    }

    static DACE::AlgebraicMatrix<DACE::DA> get_P4dydydydy_2(std::vector<std::vector<double>>& prev_4th_moment, const DACE::AlgebraicVector<DACE::DA>& dy,
                                                            unsigned int n_var)
    {
        // Auxiliary coefficients
        double coeff1;
        double coeff2;
        double coeff3;
        double coeff4;

        // Get the current workframe of the algebra
        auto n_ord_da = (unsigned long) DACE::DA::getMaxOrder();
        auto n_var_da = (unsigned long) DACE::DA::getMaxVariables();
        auto basis = tools::math::get_DA_basis((int) n_var_da, (int) n_ord_da);

        // Build resultant matrix
        DACE::AlgebraicMatrix<DACE::DA> result(n_var*n_var, n_var*n_var);
        for (int i = 0; i < n_var; i++)
        {
            for (int j = 0; j < n_var; j++)
            {
                for (int k = 0; k < n_var; k++)
                {
                    for (int l = 0; l < n_var; l++)
                    {
                        for (const auto & basi : basis)
                        {
                            // Get the constants for this base
                            coeff1 = dy[i].getCoefficient(basi);
                            coeff2 = dy[j].getCoefficient(basi);
                            coeff3 = dy[k].getCoefficient(basi);
                            coeff4 = dy[l].getCoefficient(basi);
                            result.at(i*n_var + j, k*n_var + l) += coeff1*coeff2*coeff3*coeff4;
                        }

                        result.at(i*n_var + j, k*n_var + l) *= prev_4th_moment[i * n_var + j][k * n_var + l];
                    }
                }
            }
        }
        return result;
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

    static DACE::AlgebraicMatrix<double> get_PdYdY_2(unsigned int n_var,
                            DACE::AlgebraicMatrix<double>& dy_2nd_mom,
                            DACE::AlgebraicMatrix<double>& dy_3rd_mom,
                            DACE::AlgebraicMatrix<double>& dy_4th_mom
                            )
    {
        // Auxiliary
        unsigned int idx_row;
        unsigned int idx_col;

        // Length of the final matrix
        unsigned int nrows = n_var*n_var + n_var;
        unsigned int ncols = n_var*n_var + n_var;

        // Indexes to collect
        unsigned int n_rows11 = dy_2nd_mom.nrows();
        unsigned int n_cols11 = dy_2nd_mom.ncols();

        DACE::AlgebraicMatrix<double> result(nrows, ncols);
        auto P2dy2dy2 = dy_4th_mom - tools::math::vectorize2d21d(dy_2nd_mom)*tools::math::vectorize2d21d(dy_2nd_mom).transpose();

        for (unsigned int i = 0; i < nrows; i++)
        {
            for (unsigned int j = 0; j < ncols; j++)
            {
                // Compute the index to collect 'idx_row' and 'idx_row'
                idx_row = i > n_rows11 ? i - n_rows11 : i;
                idx_col = j > n_cols11 ? j - n_cols11 : j;
                result.at(i, j) =
                        i < n_rows11 && j < n_cols11 ? dy_2nd_mom.at(idx_row, idx_col) :
                        i < n_rows11 && j > n_cols11 ? dy_3rd_mom.at(idx_row, idx_col) :
                        i > n_rows11 && j < n_cols11 ? dy_3rd_mom.at(idx_col, idx_row) :
                        P2dy2dy2.at(idx_row, idx_col);
            }
        }

        // Done!
        return result;
    }

    static DACE::AlgebraicMatrix<DACE::DA> get_P2dxdy(unsigned int n_var, DACE::AlgebraicVector<DACE::DA>& dx,
                                                     DACE::AlgebraicVector<DACE::DA>& dy,
                                                     std::vector<std::vector<double>>& prev_moments)
    {
        // Squared matrix
        DACE::AlgebraicMatrix<DACE::DA> result(n_var);

        // Fill the matrix
        for (int i = 0; i < n_var; i++)
        {
            for (int j = 0; j < n_var; j++)
            {
                result.at(i, j) = (dx[i] * dy[j]).eval(prev_moments[i]);
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
