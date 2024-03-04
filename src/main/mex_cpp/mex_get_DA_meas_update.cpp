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

            // Perform logic here -----------
            // Now we can propagate
            auto update_meas = (predicted_state_DA + sensor_noise_DA).eval(second_moments);

            // Return DA vector as str
            outputs[0] = convertNormalVector2MatlabTypedArray(update_meas);
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
};
