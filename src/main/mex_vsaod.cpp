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
        auto n_samples = convertMatlabInt2NormalInt(inputs[6]);
        auto str_alg = convertMatlabStr2NormalStr(inputs[7]);
        auto str_enum =
                str_alg == "tbp" ? PROBLEM::TWO_BODY :
                str_alg == "ftmp" ? PROBLEM::FREE_TORQUE_MOTION : PROBLEM::NA;

        double* inertia = nullptr;

        // If FTMP
        if (str_enum == PROBLEM::FREE_TORQUE_MOTION)
        {
            inertia = convertMatlab3x3Array2Normal3x3Array(inputs[8]);
        }

        // Perform operation
        auto fin_state = propagate_orbit_loads(ini_state, stddev, t, ci, nli, n_max, n_samples, str_enum, inertia);

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
        std::vector<int> int2check = {5, 6};
        std::vector<int> str2check = {7};

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
                if (inputs[i].getType() != matlab::data::ArrayType::INT16 ||
                    inputs[i].getType() == matlab::data::ArrayType::COMPLEX_INT16 ||
                    inputs[i].getNumberOfElements() != 1) {
                    print_error = true;
                }
            }


            if (print_error)
            {
                // Prepare string
                err2print = tools::string::print2string("Input %d must be of type int16", i + 1);

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
        for (auto & i : str2check)
        {
            // Safety check
            if (i >= inputs.size())
            {
                print_error = true;
            }

            if (!print_error)
            {
                if (inputs[i].getType() != matlab::data::ArrayType::MATLAB_STRING ||
                    inputs[i].getNumberOfElements() != 1) {
                    print_error = true;
                }
            }


            if (print_error)
            {
                // Prepare string
                err2print = tools::string::print2string("Input %d must be of type MATLAB_STRING", i + 1);

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
        // TODO: std::vector<double> result(array2convert.begin(), array2convert.end());
        std::vector<double> result;

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

    static int convertMatlabInt2NormalInt(const matlab::data::TypedArray<int_least16_t >& int2convert)
    {
        // Return result
        return int2convert[0];
    }

    static std::string convertMatlabStr2NormalStr(const matlab::data::TypedArray<matlab::data::MATLABString>& str2convert)
    {
        // Return result
        return std::string(str2convert[0]);
    }

    static double* convertMatlab3x3Array2Normal3x3Array(const matlab::data::TypedArray<double>& arr2convert)
    {
        double* result = (double*) malloc(sizeof (double ) * 3 * 3);

        for (int i = 0; i < 3; i++)
        {
            for (int j = 0; j < 3; j++)
            {
                result[i*j + j] = arr2convert[i][j];
            }
        }
        // Return result
        return result;
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

    std::shared_ptr<dace_array> propagate_orbit_loads(const std::vector<double>& ini_state, const std::vector<double>& stddev, const std::vector<double>& t, double& ci, double& nli, int& n_max, int& n_samples, PROBLEM prob, const double* inertia = nullptr)
    {
        // Get pointer to engine
        std::shared_ptr<matlab::engine::MATLABEngine> matlabPtr = getEngine();

        // Get array factory
        matlab::data::ArrayFactory factory;

        // Initialize DACE with 6 variables
        DACE::DA::init(2, ini_state.size());

        // Initialize beta
        std::vector<double> betas(stddev.size());
        for (int i = 0; i < stddev.size(); i++)
            betas[i] = ci * stddev[i];

        // Initialize state DA vector
        DACE::AlgebraicVector<DACE::DA> scv0(ini_state.size());
        for (int i = 0; i < ini_state.size(); i++)
            scv0[i] = ini_state[i] + betas[i] * DACE::DA(i + 1);

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

        // Initialize problem
        problems * problem;

        if (prob == PROBLEM::TWO_BODY)
        {
            problem = new problems(prob, 1);
        }
        else if (prob == PROBLEM::FREE_TORQUE_MOTION)
        {
            // Free Torque Motion problem
            problem = new problems(prob);

            // Create double[3][3]
            double inertia3x3[3][3];

            for (int i = 0; i < 3; i++)
            {
                for (int j = 0; j < 3; j++)
                {
                    inertia3x3[i][j] = *(inertia + i * j + j);
                }
            }

            // Set the inertia matrix in problem object
            problem->set_inertia_matrix(inertia3x3);
        }
        else
        {
            // Throw error
        }

        // Deduce whether interruption should be made or not
        auto interruption = true;

        // Set problem ptr in the integrator
        objIntegrator->set_problem_ptr(problem);

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

        // Set quaternion distribution stuff in case of FTMP
        if (prob == PROBLEM::FREE_TORQUE_MOTION)
        {
            // Set some options
            deltas_engine->set_bool_option(DELTA_GENERATOR_OPTION::ATTITUDE, true);
            deltas_engine->set_bool_option(DELTA_GENERATOR_OPTION::QUAT2EULER, true);
            deltas_engine->set_sampling_option(QUATERNION_SAMPLING::OMPL_GAUSSIAN);
            deltas_engine->set_mean_quaternion_option(scv0.extract(0, 3).cons());
        }

        // Set distribution
        deltas_engine->set_stddevs(stddev);

        // Compute deltas
        deltas_engine->generate_deltas(DISTRIBUTION::GAUSSIAN, n_samples);

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