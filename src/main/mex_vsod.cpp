/**
* Main to be embedded in matlab
*/
// DACE libraries
#include "dace/dace.h"

// Project libraries
#include "base/Header_Info.h"
#include "ads/SuperManifold.h"
#include "tools/io.h"

// MEX thingy
#include "mex.hpp"
#include "mexAdapter.hpp"

class MexFunction : public matlab::mex::Function {
public:
    void operator()(matlab::mex::ArgumentList outputs, matlab::mex::ArgumentList inputs) override
    {
        // Check the health of the passed inputs
        checkArguments(outputs, inputs);

        // Convert matlab array to vector of doubles
        auto doubleArray = convertMatlabTypedArray2NormalVector(inputs[0]);

        // Perform operation


        // Convert vector of doubles to matlab array
        outputs[0] = convertNormalVector2MatlabTypedArray(doubleArray);
    }

    void checkArguments(matlab::mex::ArgumentList outputs, matlab::mex::ArgumentList inputs) {
        // Get pointer to engine
        std::shared_ptr<matlab::engine::MATLABEngine> matlabPtr = getEngine();

        // Get array factory
        matlab::data::ArrayFactory factory;

        // Check offset argument: First input must be scalar double
        if (inputs[0].getType() != matlab::data::ArrayType::DOUBLE ||
            inputs[0].getType() == matlab::data::ArrayType::COMPLEX_DOUBLE ||
            inputs[0].getNumberOfElements() != 6) {
            matlabPtr->feval(u"error",
                             0,
                             std::vector<matlab::data::Array>(
                                     {factory.createScalar("First input must be a vector of 6 doubles")}));
        }


        // Check number of outputs
        if (outputs.size() > 1) {
            matlabPtr->feval(u"error",
                             0,
                             std::vector<matlab::data::Array>({factory.createScalar("Only one output is returned")}));
        }
    }


    static std::vector<double> convertMatlabTypedArray2NormalVector(const matlab::data::TypedArray<double>& inital_state)
    {
        // Set result
        std::vector<double> result{};

        // Pass to vector
        for (auto &in : inital_state) {
            result.push_back(in);
        }

        // Return result
        return result;
    }


    static matlab::data::TypedArray<double> convertNormalVector2MatlabTypedArray(const std::vector<double>& final_state)
    {
        // Get array factory
        matlab::data::ArrayFactory factory;

        // Set result
        matlab::data::TypedArray<double> result = factory.createArray<double>({final_state.size(), 1});

        // Iterate and assign
        for (int i = 0; i < final_state.size(); i++)
        {
            result[i] = final_state[i];
        }

        // Return result
        return result;
    }

    std::vector<double> propagate_loads(std::vector<double> ini_state)
    {

    }
};