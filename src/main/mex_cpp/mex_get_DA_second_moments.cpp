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
#include "ads/SuperManifold.h"
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
            auto prev_2nd_moment = mex_aux::convertMatlabTypedArray2NormalVectorArray(inputs[0]);
            auto next_state = mex_aux::convertMatlabStrVector2NormalStrVector(inputs[1]);

            // Number of variables
            int n_var = (int) next_state.size();

            // Initialize DACE with 6 variables
            DACE::DA::init(2, n_var);

            // Convert state to DA vector
            auto next_state_DA = DACE::AlgebraicVector<DACE::DA>(n_var);
            for (int i = 0; i < n_var; i++) {
                next_state_DA[i] = DACE::DA::fromString(next_state[i]);
            }

            // Print previous moments
            // std::fprintf(stdout, "%s\n", tools::vector::num2string(prev_2nd_moment).c_str());

            // Perform logic here -----------
            // Now we can propagate
            // auto next_2nd_moments = get_Pxx(prev_2nd_moment, next_state_DA);

            // Return DA vector as str
            outputs[0] = get_Pxx(prev_2nd_moment, next_state_DA);
        }
        catch (int) {
            std::fprintf(stdout, "ERROR: Something unexpected happened...");
        }
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

    matlab::data::TypedArray<double> get_Pxx(std::vector<std::vector<double>>& prev_2nd_moment, const DACE::AlgebraicVector<DACE::DA>& next_state_DA)
    {
        // Auxiliary coefficients
        double coeff1;
        double coeff2;

        // Get the dimension of the propagation
        auto n_var_problem = next_state_DA.size();

        // Get the current workframe of the algebra
        auto n_ord_da = (unsigned long) DACE::DA::getMaxOrder();
        auto n_var_da = (unsigned long) DACE::DA::getMaxVariables();

        // Generate result vector
        matlab::data::TypedArray<double> result = this->factoryPtr->createArray<double>(
                {n_var_problem, n_var_problem});

        // Get the basis generator of the algebra
        auto basis = tools::math::get_DA_basis((int) n_var_da, (int) n_ord_da);

        // Iterate through all the variables
        for (int i = 0; i < n_var_problem; i++)
        {
            for (int j = 0; j < n_var_problem; j++)
            {
                for (const auto & basi : basis)
                {
                    // Get the constants for this base
                    coeff1 = next_state_DA[i].getCoefficient(basi);
                    coeff2 = next_state_DA[j].getCoefficient(basi);
                    result[i][j] += coeff1*coeff2;
                }
                result[i][j] *= prev_2nd_moment[i][j];
            }
        }

        // std::fprintf(stdout, "%s\n", tools::vector::num2string(result).c_str());
        // Done!
        return result;
    }
};
