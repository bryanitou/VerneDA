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
            auto state_str = mex_aux::convertMatlabStrVector2NormalStrVector(inputs[0]);
            DACE::DA::init(2, state_str.size());

            // Convert state to DA vector
            auto state_DA = DACE::AlgebraicVector<DACE::DA>(state_str.size());

            for (int i = 0; i < state_str.size(); i++)
            {
                state_DA[i] = DACE::DA::fromString(state_str[i]);
            }

            // Perform logic here -----------
            // Initialize DACE with 6 variables


            // Return DA vector as str
            outputs[0] = this->convertNormalStr2MatlabStr("failed");
        }
        catch (int) {
            std::fprintf(stdout, "ERROR: Something unexpected happened...");
        }
    }

    matlab::data::TypedArray<matlab::data::MATLABString>
    convertNormalStr2MatlabStr(const std::string &c_plus_plus_string) {
        std::u16string u16_String{};

        mex_aux::fromUTF8(c_plus_plus_string, u16_String);

        // Return result
        return this->factoryPtr->createArray<matlab::data::MATLABString>({1, 1},
                                                                         {matlab::data::MATLABString(
                                                                                 u16_String.c_str())});
    }
};
