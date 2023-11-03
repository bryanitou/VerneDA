/**
* Main to be embedded in matlab
*/
// DACE libraries
#include "dace/dace.h"

// Project libraries
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
        auto ini_state = convertMatlabTypedArray2NormalVector(inputs[0]);
        auto betas = convertMatlabTypedArray2NormalVector(inputs[1]);

        // Perform operation
        auto fin_state =  propagate_loads(ini_state, betas);

        // Convert vector of doubles to matlab array
        outputs[0] = convertNormalVector2MatlabTypedArray(fin_state);
    }

    void checkArguments(matlab::mex::ArgumentList outputs, matlab::mex::ArgumentList inputs) {
        // Get pointer to engine
        std::shared_ptr<matlab::engine::MATLABEngine> matlabPtr = getEngine();

        // Get array factory
        matlab::data::ArrayFactory factory;

        // Check offset argument: First input must be scalar double
        for (auto & in : inputs)
        {
            if (in.getType() != matlab::data::ArrayType::DOUBLE ||
                in.getType() == matlab::data::ArrayType::COMPLEX_DOUBLE ||
                in.getNumberOfElements() != 6) {
                matlabPtr->feval(u"error",
                                 0,
                                 std::vector<matlab::data::Array>(
                                         {factory.createScalar("Inputs must be a vector of 6 doubles")}));
            }


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

    std::vector<double> propagate_loads(const std::vector<double>& ini_state, const std::vector<double>& betas) {
        // Get pointer to engine
        std::shared_ptr<matlab::engine::MATLABEngine> matlabPtr = getEngine();

        // Get array factory
        matlab::data::ArrayFactory factory;

        // Initialize DACE with 6 variables
        DACE::DA::init(2, 6);

        // Initialize state DA vector
        DACE::AlgebraicVector<DACE::DA> scv0 = {
                ini_state[0] + betas[0] * DACE::DA(1),
                ini_state[1] + betas[1] * DACE::DA(2),
                ini_state[2] + betas[2] * DACE::DA(3),
                ini_state[3] + betas[3] * DACE::DA(4),
                ini_state[4] + betas[4] * DACE::DA(5),
                ini_state[5] + betas[5] * DACE::DA(6)
        };

        matlabPtr->feval(u"fprintf",
                         0,
                         std::vector<matlab::data::Array>({factory.createScalar(scv0.toString())}));

        return ini_state;
    }
};