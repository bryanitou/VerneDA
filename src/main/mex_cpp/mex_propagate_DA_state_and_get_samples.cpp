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
            auto state_str = mex_aux::convertMatlabStrVector2NormalStrVector(inputs[0]);
            auto stddev = mex_aux::convertMatlabTypedArray2NormalVector(inputs[1]);
            auto betas = mex_aux::convertMatlabTypedArray2NormalVector(inputs[2]);
            auto t = mex_aux::convertMatlabTypedArray2NormalVector(inputs[3]);
            auto nli = mex_aux::convertMatlabDouble2NormalDouble(inputs[4]);
            auto n_samples = mex_aux::convertMatlabDouble2NormalDouble(inputs[5]);

            // Initialize DACE with 6 variables
            DACE::DA::init(2, state_str.size());

            // Convert state to DA vector
            auto state_DA = DACE::AlgebraicVector<DACE::DA>(state_str.size());

            for (int i = 0; i < state_str.size(); i++) {
                state_DA[i] = DACE::DA::fromString(state_str[i]);
            }

            // Perform logic here -----------
            // Now we can propagate
            auto final_manifold = propagate_orbit_loads_SUPER_MANIFOLD(state_DA, betas, t, nli);
            auto samples = get_samples(final_manifold, stddev, n_samples);

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

    static SuperManifold *
    propagate_orbit_loads_SUPER_MANIFOLD(const DACE::AlgebraicVector<DACE::DA> scv, std::vector<double> betas, std::vector<double> t, double nli)
    {
        // Initial and final time and time step
        double const t0 = t[0];
        double const tf = t[1];
        double const dt = t[2];

        // Initialize integrator
        auto objIntegrator = std::make_unique<integrator>(INTEGRATOR::RK4, ALGORITHM::LOADS, dt);

        // Build super manifold: LOADS
        auto super_manifold = new SuperManifold(nli, 10, ALGORITHM::LOADS);

        // Set beta constant in integrator
        objIntegrator->set_beta(betas);

        // Initialize problem
        problems *problem;
        problem = new problems(PROBLEM::TWO_BODY, 1);

        // Set problem ptr in the integrator
        objIntegrator->set_problem_ptr(problem);

        // Setting integrator parameters
        objIntegrator->set_integration_parameters(scv, t0, tf, false);

        // Set integrator in the super manifold
        super_manifold->set_integrator_ptr(objIntegrator.get());

        // Docu: Set new truncation error and get the previous one
        double new_eps = 1e-40;
        DACE::DA::setEps(new_eps);

        // Apply main algorithm: ADS / LOADS. And integration algorithm
        // std::string prop_summary{};
        // super_manifold->split_domain(&prop_summary);
        super_manifold->split_domain();

        return super_manifold;
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
