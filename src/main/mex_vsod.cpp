/**
* Main to be embedded in matlab
*/
// DACE libraries
#include "dace/dace.h"

// Project libraries
#include "ads/SuperManifold.h"
#include "delta.h"

// MEX thingy
#include "mex.hpp"
#include "mexAdapter.hpp"

// Some definitions
# define dace_array std::vector<DACE::AlgebraicVector<double>>

class MexFunction : public matlab::mex::Function {
public:
    void operator()(matlab::mex::ArgumentList outputs, matlab::mex::ArgumentList inputs) override
    {
        // Check the health of the passed inputs
        checkArguments(outputs, inputs);

        // Extract inputs
        auto ini_state = convertMatlabTypedArray2NormalVector(inputs[0]);
        auto stddev = convertMatlabTypedArray2NormalVector(inputs[1]);
        auto t = convertMatlabTypedArray2NormalVector(inputs[2]);
        auto ci = convertMatlabDouble2NormalDouble(inputs[3]);
        auto nli = convertMatlabDouble2NormalDouble(inputs[4]);
        auto n_max = convertMatlabInt2NormalInt(inputs[5]);

        // Perform operation
        auto fin_state = propagate_loads(ini_state, stddev, t, ci, nli, n_max);

        // Convert all states to matlab array
        outputs[0] = convertNormalVector2MatlabTypedArray(*fin_state);
    }

    void checkArguments(matlab::mex::ArgumentList outputs, matlab::mex::ArgumentList inputs) {
        // Auxiliary variable
        std::string err2print;
        bool print_error = false;

        // Get pointer to engine
        std::shared_ptr<matlab::engine::MATLABEngine> matlabPtr = getEngine();

        // Get array factory
        matlab::data::ArrayFactory factory;

        // Define vectors to check the inputs
        std::vector<int> vectors2check6 = {0, 1};
        std::vector<int> vectors2check3 = {2};
        std::vector<int> doubles2check = {3, 4};
        std::vector<int> int2check = {5};

        // Check arguments
        for (auto & i : vectors2check6)
        {
            // Safety check
            if (i >= inputs.size())
            {
                print_error = true;
            }

            if (!print_error)
            {
                if (inputs[i].getType() != matlab::data::ArrayType::DOUBLE ||
                    inputs[i].getType() == matlab::data::ArrayType::COMPLEX_DOUBLE ||
                    inputs[i].getNumberOfElements() != 6) {
                    print_error = true;
                }
            }

            if (print_error)
            {
                // String to show
                err2print = tools::string::print2string("Input %d must be an array of 6 doubles", i + 1);


                // Show string
                matlabPtr->feval(u"error",
                                 0,
                                 std::vector<matlab::data::Array>(
                                         {factory.createScalar(err2print)}));
            }
        }

        // Check arguments
        for (auto & i : vectors2check3)
        {
            // Safety check
            if (i >= inputs.size())
            {
                print_error = true;
            }

            if (!print_error)
            {
                if (inputs[i].getType() != matlab::data::ArrayType::DOUBLE ||
                    inputs[i].getType() == matlab::data::ArrayType::COMPLEX_DOUBLE ||
                    inputs[i].getNumberOfElements() != 3) {
                    print_error = true;
                }
            }

            if (print_error)
            {
                // String to show
                err2print = tools::string::print2string("Input %d must be an array of 3 doubles", i + 1);


                // Show string
                matlabPtr->feval(u"error",
                                 0,
                                 std::vector<matlab::data::Array>(
                                         {factory.createScalar(err2print)}));
            }
        }

        // Check arguments
        for (auto & i : doubles2check)
        {
            // Safety check
            if (i >= inputs.size())
            {
                print_error = true;
            }

            if (!print_error)
            {
                if (inputs[i].getType() != matlab::data::ArrayType::DOUBLE ||
                    inputs[i].getType() == matlab::data::ArrayType::COMPLEX_DOUBLE ||
                    inputs[i].getNumberOfElements() != 1) {
                    print_error = true;
                }
            }


            if (print_error)
            {
                // Prepare string
                err2print = tools::string::print2string("Input %d must be of type double", i + 1);

                // Show error
                matlabPtr->feval(u"error",
                                 0,
                                 std::vector<matlab::data::Array>(
                                         {factory.createScalar(err2print)}));

                // Reset print error
                print_error = false;
            }
        }

        // Check arguments
        for (auto & i : int2check)
        {
            // Safety check
            if (i >= inputs.size())
            {
                print_error = true;
            }

            if (!print_error)
            {
                if (inputs[i].getType() != matlab::data::ArrayType::INT8 ||
                    inputs[i].getType() == matlab::data::ArrayType::COMPLEX_INT8 ||
                    inputs[i].getNumberOfElements() != 1) {
                    print_error = true;
                }
            }


            if (print_error)
            {
                // Prepare string
                err2print = tools::string::print2string("Input %d must be of type int8", i + 1);

                // Show error
                matlabPtr->feval(u"error",
                                 0,
                                 std::vector<matlab::data::Array>(
                                         {factory.createScalar(err2print)}));

                // Reset print error
                print_error = false;
            }
        }

        // Check number of outputs
        if (outputs.size() > 1) {
            matlabPtr->feval(u"error",
                             0,
                             std::vector<matlab::data::Array>({factory.createScalar("Only one output is returned")}));
        }
    }


    static std::vector<double> convertMatlabTypedArray2NormalVector(const matlab::data::TypedArray<double>& array2convert)
    {
        // Set result
        std::vector<double> result{};

        // Pass to vector
        for (auto &in : array2convert)
        {
            result.push_back(in);
        }

        // Return result
        return result;
    }

    static double convertMatlabDouble2NormalDouble(const matlab::data::TypedArray<double>& double2convert)
    {
        // Return result
        return double2convert[0];
    }

    static int convertMatlabInt2NormalInt(const matlab::data::TypedArray<int_fast8_t >& int2convert)
    {
        // Return result
        return int2convert[0];
    }


    static matlab::data::TypedArray<double> convertNormalVector2MatlabTypedArray(const dace_array& final_state)
    {
        // Get array factory
        matlab::data::ArrayFactory factory;

        // Set result
        matlab::data::TypedArray<double> result = factory.createArray<double>({final_state[0].size(), final_state.size()});

        // Iterate and assign
        for (int i = 0; i < final_state.size(); i++)
        {
            for (int j = 0; j < final_state[j].size(); j++)
            {
                result[j][i] = final_state[i][j];
            }
        }

        // Return result
        return result;
    }

    std::shared_ptr<dace_array> propagate_loads(const std::vector<double>& ini_state, const std::vector<double>& stddev, const std::vector<double>& t, double& ci, double& nli, int& n_max)
    {
        // Get pointer to engine
        std::shared_ptr<matlab::engine::MATLABEngine> matlabPtr = getEngine();

        // Get array factory
        matlab::data::ArrayFactory factory;

        // Initialize DACE with 6 variables
        DACE::DA::init(2, 6);

        // Initialize beta
        std::vector<double> betas =
                {
                        ci * stddev[0],
                        ci * stddev[1],
                        ci * stddev[2],
                        ci * stddev[3],
                        ci * stddev[4],
                        ci * stddev[5]
                };

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

        // Initial and final time and time step
        double const t0 = t[0];
        double const tf = t[1];
        double const dt = t[2];

        // Initialize integrator
        auto objIntegrator = std::make_unique<integrator>(INTEGRATOR::RK4, ALGORITHM::LOADS, dt);

        // Build super manifold: LOADS
        auto super_manifold = new SuperManifold(nli, n_max, ALGORITHM::LOADS);

        // Set beta constant in integrator
        objIntegrator->set_beta(const_cast<std::vector<double> &>(betas));

        // Set time scaling
        objIntegrator->set_time_scaling(2444.887594345139);

        // Initialize problem
        auto prob = new problems(PROBLEM::TWO_BODY, 1.0);

        // Deduce whether interruption should be made or not
        auto interruption = true;

        // Set problem ptr in the integrator
        objIntegrator->set_problem_ptr(prob);

        // Setting integrator parameters
        objIntegrator->set_integration_parameters(scv0, t0, tf,interruption);

        // Set integrator in the super manifold
        super_manifold->set_integrator_ptr(objIntegrator.get());

        // Docu: Set new truncation error and get the previous one
        double new_eps = 1e-40;
        DACE::DA::setEps(new_eps);

        // Apply main algorithm: ADS / LOADS. And integration algorithm
        super_manifold->split_domain();

        // Build deltas class
        auto deltas_engine = std::make_shared<delta>();

        // Set distribution
        deltas_engine->set_stddevs(stddev);

        // Compute deltas
        deltas_engine->generate_deltas(DISTRIBUTION::GAUSSIAN, 10000);

        // Insert nominal delta
        deltas_engine->insert_nominal(static_cast<int>(ini_state.size()));

        // Set super manifold in deltas engine
        deltas_engine->set_superManifold(super_manifold);

        // Evaluate deltas
        deltas_engine->evaluate_deltas();

        // Get result
        return deltas_engine->get_eval_deltas_poly();
    }
};