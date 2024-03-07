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
            auto new_1st_moment = mex_aux::convertMatlabTypedArray2NormalVector(inputs[0]);
            auto pred_state = mex_aux::convertMatlabStrVector2NormalStrVector(inputs[1]);
            auto sensor = mex_aux::convertMatlabStrVector2NormalStrVector(inputs[2]);

            // Number of variables
            int n_var = (int) pred_state.size();

            // Initialize DACE with 6 variables
            DACE::DA::init(2, n_var);

            // Convert state to DA vector
            auto err_var_pred_DA = DACE::AlgebraicVector<DACE::DA>(n_var);
            for (int i = 0; i < n_var; i++) {
                err_var_pred_DA[i] =
                       - new_1st_moment[i] + DACE::DA::fromString(pred_state[i]) + DACE::DA::fromString(sensor[i]);
            }

            // Return DA vector as str
            outputs[0] = convertDAVector2MatlabStr(err_var_pred_DA);
        }
        catch (int) {
            std::fprintf(stdout, "ERROR: Something unexpected happened...");
        }
    }

    template<class T>
    matlab::data::TypedArray<matlab::data::MATLABString> convertDAVector2MatlabStr(const DACE::AlgebraicVector<T>& da_vector)
    {
        std::u16string u16_String{};
        // Attention: this line is sensitive to MATLAB versions
        auto result = this->factoryPtr->createArray<matlab::data::MATLABString>({da_vector.size(), 1});

        for (unsigned int i = 0; i < (unsigned int ) da_vector.size(); i++)
        {
            // Convert format
            mex_aux::fromUTF8(da_vector[i].toString(), u16_String);
            // Fill result
            result[i] = u16_String;
        }
        return result;
    }
};
