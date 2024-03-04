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
            auto prev_state = mex_aux::convertMatlabStrVector2NormalStrVector(inputs[0]);
            auto next_state = mex_aux::convertMatlabStrVector2NormalStrVector(inputs[1]);
            auto measurement_str = mex_aux::convertMatlabStrVector2NormalStrVector(inputs[2]);
            auto n_step = mex_aux::convertMatlab32Int2NormalInt(inputs[3]);

            // Number of variables
            int n_var = (int) prev_state.size();

            // Initialize DACE with 6 variables
            DACE::DA::init(2, n_var);

            // Convert state to DA vector
            auto prev_state_DA = DACE::AlgebraicVector<DACE::DA>(n_var);
            for (int i = 0; i < n_var; i++) {
                prev_state_DA[i] = DACE::DA::fromString(prev_state[i]);
            }
            auto next_state_DA = DACE::AlgebraicVector<DACE::DA>(n_var);
            for (int i = 0; i < n_var; i++) {
                next_state_DA[i] = DACE::DA::fromString(next_state[i]);
            }

            // Measurement DA
            auto measurement_DA = DACE::AlgebraicVector<DACE::DA>(n_var);
            for (int i = 0; i < n_var; i++) {
                measurement_DA[i] = DACE::DA::fromString(measurement_str[i]);
            }

            // Perform logic here -----------
            // Now we can propagate
            HODAKF_filter(prev_state_DA, next_state_DA, measurement_DA, n_step);

            // Return DA vector as str
            // TODO: Fix this
            outputs[0] = inputs[0];
        }
        catch (int) {
            std::fprintf(stdout, "ERROR: Something unexpected happened...");
        }
    }


    auto get_Pxx_gaussian(const DACE::AlgebraicVector<DACE::DA>& state)
    {
        // Look up in the moments table
        // Pxx are the second central moments because it is the E { ( [X] - \mu_x ) ^ 2 }
        // https://en.wikipedia.org/wiki/Normal_distribution#Moments
        // Therefore: mean squared + variance squared
        // The state is composed like:
        // x_1(t=0) = mean_1 + ci * stddev_1 * DA(1)
        // x_2(t=0) = mean_2 + ci * stddev_1 * DA(2)
        // ...
        // x_N(t=0) = mean_N + ci * stddev_N * DA(N)
        // Then:
        // variance_N = (ci * stddev_N)²

        // Result
        DACE::AlgebraicVector<DACE::DA> result(state.size());

        // Aux. vals
        double var;
        double cons;

        for (int i = 0; i < state.size(); i++)
        {
            cons = state[i].cons();
            var = state[i].linear()[0];
            result[i] = cons*cons + var*var;
        }
        return result;
    }


    void get_Pxx(const DACE::AlgebraicVector<DACE::DA>& state)
    {

        // Order of the expansion: 2
        auto n_ord = (int) DACE::DA::getMaxOrder();
        auto n_var = (int) DACE::DA::getMaxVariables();

        // Debugging string
        std::string debugging_str{"Debugging string start here:\n"};

        DACE::AlgebraicVector<DACE::DA> result(n_var);

        // Vector of zeros
        DACE::AlgebraicVector<DACE::DA> v_list(n_var, 0.0);

        // Create vector of ones
        std::vector<unsigned int> ones(n_var, 0);
        ones[0] = 1;
        // Iterate through all the variables
        for (int i = 0; i < state.size(); i++)
        {
            auto var = state[i];
            DACE::DA cov_i = 0;
            // std::fprintf(stdout, "ones: size: %zu val: %s ... \n", ones.size(), tools::vector::num2string(ones).c_str());
            // auto coeffs = var.getCoefficient(ones);
            // std::fprintf(stdout, "Entering loop... \n");
            // debugging_str += "coeffs: " + std::to_string(coeffs) + "\n";
            // Iterate through all the monomials
            for (auto & x_ith_mono : var.getMonomials())
            {
                cov_i += x_ith_mono.m_coeff * x_ith_mono.m_coeff * DACE::DA(i + 1) * DACE::DA(i + 1);
                debugging_str += "mono: " + x_ith_mono.toString() + " ";
            }

            result[i] = cov_i;
            debugging_str += "\n\n";
        }

        // Print result
        debugging_str += result.toString();

        // Print
        debugging_str += "Debugging string finishes here ---\n";
        std::fprintf(stdout, "%s", debugging_str.c_str());
    }


    void HODAKF_filter(const DACE::AlgebraicVector<DACE::DA>& prev_state,
                       const DACE::AlgebraicVector<DACE::DA>& next_state,
                       const DACE::AlgebraicVector<DACE::DA>& measurement, int n_step)
    {
        // Debugging string
        std::string debugging_str{"HODAKF_filter: Debugging string start here:\n"};

        // 1. Evaluate predicted measurement mean and covariance
        // For that, we need the Pxx
        DACE::AlgebraicVector<DACE::DA> Pxx;
        if (n_step == 0)
        {
            Pxx = get_Pxx_gaussian(prev_state);
        }
        else
        {
            get_Pxx(prev_state);
        }

        // Print
        debugging_str += tools::string::print2string("HODAKF_filter: %s \n", Pxx.toString().c_str());
        debugging_str += "HODAKF_filter: Debugging string finishes here ---\n";
        std::fprintf(stdout, "%s", debugging_str.c_str());
    }
};
