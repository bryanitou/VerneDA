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
            auto prev_1st_moment = mex_aux::convertMatlabTypedArray2NormalVector(inputs[0]);
            auto pred_state = mex_aux::convertMatlabStrVector2NormalStrVector(inputs[1]);
            auto noise = mex_aux::convertMatlabStrVector2NormalStrVector(inputs[2]);

            // Number of variables
            int n_var = (int) pred_state.size();

            // Initialize DACE with 6 variables
            DACE::DA::init(2, n_var);

            // Convert state to DA vector
            auto nx_noisy_state_DA = DACE::AlgebraicVector<DACE::DA>(n_var);
            for (int i = 0; i < n_var; i++) {
                nx_noisy_state_DA[i] = DACE::DA::fromString(pred_state[i]) + DACE::DA::fromString(noise[i]);
            }

            // Return DA vector as str
            outputs[0] = get_1st_Mom(prev_1st_moment, nx_noisy_state_DA);
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


    matlab::data::TypedArray<double> get_1st_Mom(std::vector<double>& prev_1st_moment, DACE::AlgebraicVector<DACE::DA> next_state_DA)
    {
        // Get the dimension of the propagation
        auto n_var_problem = next_state_DA.size();

        // Get the current workframe of the algebra
        auto n_ord_da = (unsigned long) DACE::DA::getMaxOrder();
        auto n_var_da = (unsigned long) DACE::DA::getMaxVariables();

        // Generate result vector
        matlab::data::TypedArray<double> result = this->factoryPtr->createArray<double>({1, n_var_problem});

        // Get the basis generator of the algebra
        auto basis = tools::math::get_DA_basis((int) n_var_da, (int) n_ord_da);

        // Remove the constant part
        next_state_DA = next_state_DA - next_state_DA.cons();

        // Iterate through all the variables
        for (int i = 0; i < n_var_problem; i++)
        {
            for (const auto & basi : basis)
            {
                // Get the constants for this base
                result[i] += next_state_DA[i].getCoefficient(basi);
            }
            result[i] *= prev_1st_moment[i];

        }

        // Done!
        return result;
    }
};
