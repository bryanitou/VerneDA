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
            auto prev_2nd_moment = mex_aux::convertMatlabTypedArray2NormalVector(inputs[0]);
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
            std::fprintf(stdout, "%s\n", tools::vector::num2string(prev_2nd_moment).c_str());

            // Perform logic here -----------
            // Now we can propagate
            auto next_2nd_moments = get_Pxx(prev_2nd_moment, next_state_DA);

            // Return DA vector as str
            outputs[0] = convertNormalVector2MatlabTypedArray(next_2nd_moments);
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

    static std::vector<double> get_Pxx(std::vector<double>& prev_2nd_moment, const DACE::AlgebraicVector<DACE::DA>& next_state_DA)
    {
        // Auxiliary coefficients
        double coeff;
        double coeff_sq;

        // Get the current workframe of the algebra
        auto n_ord = (int) DACE::DA::getMaxOrder();
        auto n_var = (int) DACE::DA::getMaxVariables();

        // Generate result vector
        std::vector<double> result(n_var, 0.0);

        // Get the basis generator of the algebra
        auto basis = tools::math::get_DA_basis(n_var, n_ord);

        // Iterate through all the variables
        for (int i = 0; i < next_state_DA.size(); i++)
        {
            // For this variable, iterate through all its possible contributors
            for (int j = 0; j < basis.size(); j++)
            {
                // Get the constants for this base
                coeff = next_state_DA[i].getCoefficient(basis[j]);
                coeff_sq = coeff*coeff;
                result[i] += prev_2nd_moment[i] * coeff_sq;
            }
        }

        std::fprintf(stdout, "%s\n", tools::vector::num2string(result).c_str());
        // Done!
        return result;
    }
};
