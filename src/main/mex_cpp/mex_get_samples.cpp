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
#include "delta.h"

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
            auto state_DA_ini_str = mex_aux::convertMatlabStrVector2NormalStrVector(inputs[0]);
            auto state_DA_fin_str = mex_aux::convertMatlabStrArray2NormalStrVector(inputs[1]);
            auto stddev = mex_aux::convertMatlabTypedArray2NormalVector(inputs[2]);
            auto n_samples = mex_aux::convertMatlabDouble2NormalDouble(inputs[3]);

            // Initialize DACE with 6 variables
            DACE::DA::init(2, state_DA_ini_str.size());

            // Convert state to DA vector
            auto state_DA_ini = DACE::AlgebraicVector<DACE::DA>(state_DA_ini_str.size());
            auto state_DA_aux = DACE::AlgebraicVector<DACE::DA>(state_DA_fin_str.front().size());
            std::fprintf(stdout, "it works here 48, state_DA_aux: %zu\n", state_DA_aux.size());
            std::fprintf(stdout, "it works here 49, state_DA_ini: %zu\n", state_DA_ini.size());
            for (int i = 0; i < state_DA_ini_str.size(); i++) {
                std::fprintf(stdout, "it works here 41, state_DA_ini_str[%d]:\n%s\n", i, state_DA_ini_str[i].c_str());
                state_DA_ini[i] = DACE::DA::fromString(state_DA_ini_str[i]);
            }
            std::fprintf(stdout, "it works here 54\n");
            auto mani_ini = Manifold(state_DA_ini);
            std::fprintf(stdout, "it works here 56\n");

            // Final manifold
            auto mani_fin = Manifold();
            for (int i = 0; i < state_DA_fin_str.size(); i++)
            {
                for (int j = 0; j < state_DA_fin_str.front().size(); j++)
                {
                    // std::fprintf(stdout, "it works here 64, state_DA_fin_str[%d]:\n%s\n", i, state_DA_fin_str[i].c_str());
                    state_DA_aux[j] = DACE::DA::fromString(state_DA_fin_str[i][j]);
                }
                mani_fin.push_back(state_DA_aux);
            }
            std::cout << state_DA_aux << std::endl;
            std::fprintf(stdout, "it works here 70\n");
            auto super_manifold = new SuperManifold(ALGORITHM::LOADS);
            super_manifold->current_ = &mani_fin;
            super_manifold->previous_ = &mani_ini;

            // Perform logic here -----------
            // Now we can propagate
            std::fprintf(stdout, "it works here 77\n");
            auto samples = get_samples(super_manifold, stddev, n_samples);

            // Return DA vector as str
            outputs[0] = this->convertNormalVector2MatlabTypedArray(*samples);
        }
        catch (int) {
            std::fprintf(stdout, "ERROR: Something unexpected happened...");
        }
    }

    matlab::data::TypedArray<double>
    convertNormalVector2MatlabTypedArray(const std::vector<DACE::AlgebraicVector<double>> &final_state) {
        // Set result
        matlab::data::TypedArray<double> result = this->factoryPtr->createArray<double>(
                {final_state[0].size(), final_state.size()});

        // Iterate and assign
        for (int i = 0; i < final_state.size(); i++) {
            for (int j = 0; j < final_state[j].size(); j++) {
                result[j][i] = final_state[i][j];
            }
        }

        // Return result
        return result;
    }



    matlab::data::TypedArray<matlab::data::MATLABString> convertManifoldVector2MatlabStr(Manifold* manifold)
    {
        std::vector<DACE::DA> da_vector;
        std::u16string u16_String{};
        // Attention: this line is sensitive to MATLAB versions
        auto result = this->factoryPtr->createArray<matlab::data::MATLABString>({manifold->front().size(), manifold->size()});

        for (int j = 0; j < manifold->size(); j++)
        {
            // Get DA vector
            da_vector = manifold->at(j);

            for (unsigned int i = 0; i < (unsigned int ) da_vector.size(); i++)
            {
                // Convert format
                mex_aux::fromUTF8(da_vector[i].toString(), u16_String);
                // Fill result
                result[i][j] = u16_String;
            }
        }

        return result;
    }


    static std::shared_ptr<std::vector<DACE::AlgebraicVector<double>>>
            get_samples(SuperManifold* sm, std::vector<double> stddev, unsigned int n_samples)
    {
        // Build deltas class
        auto deltas_engine = std::make_shared<delta>();

        // Set distribution
        deltas_engine->set_stddevs(stddev);

        // Compute deltas
        deltas_engine->generate_deltas(DISTRIBUTION::GAUSSIAN, n_samples);

        // Insert nominal delta, this is the center
        deltas_engine->insert_nominal(DACE::AlgebraicVector<double> (6));

        // Set super manifold in deltas engine
        deltas_engine->set_superManifold(sm);

        // Evaluate deltas
        deltas_engine->evaluate_deltas();

        // Get result
        return deltas_engine->get_eval_deltas_poly();
    }
};
