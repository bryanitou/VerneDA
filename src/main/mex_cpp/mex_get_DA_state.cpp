/**
 * Only return DA vector
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
            auto ini_state = mex_aux::convertMatlabTypedArray2NormalVector(inputs[0]);
            auto stddev = mex_aux::convertMatlabTypedArray2NormalVector(inputs[1]);
            auto ci = mex_aux::convertMatlabDouble2NormalDouble(inputs[2]);

            // Perform logic here -----------
            // Initialize DACE with 6 variables
            DACE::DA::init(2, ini_state.size());

            // Initialize state DA vector
            auto scv0 = DACE::AlgebraicVector<DACE::DA>(ini_state.size());
            for (int i = 0; i < ini_state.size(); i++) { scv0[i] = ini_state[i] + ci * stddev[i] * DACE::DA(i + 1); }
            // Return DA vector as str
            outputs[0] = this->convertDAVector2MatlabStr(scv0);
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
        auto result = this->factoryPtr->createArray<matlab::data::MATLABString>({6, 1});

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