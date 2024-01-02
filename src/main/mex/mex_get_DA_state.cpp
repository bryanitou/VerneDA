/**
* Main to be embedded in matlab


// MEX thingy
#include "mex.hpp"
#include "mexAdapter.hpp"

// DACE libraries
#include "dace/dace.h"

// Mex auxiliary
#include "tools/mex_aux.h"

// Project libraries
#include "ads/SuperManifold.h"
#include "delta.h"

class MexFunction : public matlab::mex::Function {

private:
    // Set some variables used in the functions
    std::vector<double> betas;
    DACE::AlgebraicVector<DACE::DA> scv0;

public:
    void operator()(matlab::mex::ArgumentList outputs, matlab::mex::ArgumentList inputs) override {
      try {

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

        double *inertia = nullptr;

        // If FTMP
        if (str_enum == PROBLEM::FREE_TORQUE_MOTION) {
          inertia = aux.convertMatlab3x3Array2Normal3x3Array(inputs[8]);
        }

        // Simple DA propagation
        if (n_max == - 1) {
          // Info
          this->matlab_ptr_->feval(u"fprintf",
                                   0,
                                   std::vector<matlab::data::Array>(
                                           {this->factory_ptr_->createScalar("DA Simple propagation selected...\n")}));

          // Perform operation
          auto fin_state = get_DA(ini_state, stddev, t, ci, nli, n_max, n_samples, str_enum,
                                                        inertia);

          // Convert all states to matlab array
          outputs[0] = aux.convertNormalStr2MatlabStr(fin_state);
        } else // LOADS propagation
        {
          // Perform operation
          auto fin_state = propagate_orbit_loads(ini_state, stddev, t, ci, nli, n_max, n_samples, str_enum,
                                                 inertia);

          // Convert all states to matlab array
          outputs[0] = aux.convertNormalVector2MatlabTypedArray(*fin_state);
        }


      }
      catch (int) {
        std::fprintf(stdout, "ERROR: Something unexpected happened...");
      }
    }

    void checkArguments(matlab::mex::ArgumentList outputs, matlab::mex::ArgumentList inputs) {
      // Auxiliary variable
      std::string err2print;
      bool print_error = false;

      // Define vectors to check the inputs
      std::vector<int> vectors2check6 = {0, 1};
      std::vector<int> vectors2check3 = {2};
      std::vector<int> doubles2check = {3, 4};
      std::vector<int> int2check = {5, 6};
      std::vector<int> str2check = {7};

      // Check arguments
      for (auto &i: vectors2check6) {
        // Safety check
        if (i >= inputs.size()) {
          print_error = true;
        }

        if (! print_error) {
          if (inputs[i].getType() != matlab::data::ArrayType::DOUBLE ||
              inputs[i].getType() == matlab::data::ArrayType::COMPLEX_DOUBLE ||
              (inputs[i].getNumberOfElements() != 6 && inputs[i].getNumberOfElements() != 7)) {
            print_error = true;
          }
        }

        if (print_error) {
          // String to show
          err2print = tools::string::print2string("Input %d must be an array of 6 doubles", i + 1);


          // Show string
          this->matlab_ptr_->feval(u"error",
                                   0,
                                   std::vector<matlab::data::Array>(
                                           {this->factory_ptr_->createScalar(err2print)}));
        }
      }

      // Check arguments
      for (auto &i: vectors2check3) {
        // Safety check
        if (i >= inputs.size()) {
          print_error = true;
        }

        if (! print_error) {
          if (inputs[i].getType() != matlab::data::ArrayType::DOUBLE ||
              inputs[i].getType() == matlab::data::ArrayType::COMPLEX_DOUBLE ||
              inputs[i].getNumberOfElements() != 3) {
            print_error = true;
          }
        }

        if (print_error) {
          // String to show
          err2print = tools::string::print2string("Input %d must be an array of 3 doubles", i + 1);


          // Show string
          this->matlab_ptr_->feval(u"error",
                                   0,
                                   std::vector<matlab::data::Array>(
                                           {this->factory_ptr_->createScalar(err2print)}));
        }
      }

      // Check arguments
      for (auto &i: doubles2check) {
        // Safety check
        if (i >= inputs.size()) {
          print_error = true;
        }

        if (! print_error) {
          if (inputs[i].getType() != matlab::data::ArrayType::DOUBLE ||
              inputs[i].getType() == matlab::data::ArrayType::COMPLEX_DOUBLE ||
              inputs[i].getNumberOfElements() != 1) {
            print_error = true;
          }
        }


        if (print_error) {
          // Prepare string
          err2print = tools::string::print2string("Input %d must be of type double", i + 1);

          // Show error
          this->matlab_ptr_->feval(u"error",
                                   0,
                                   std::vector<matlab::data::Array>(
                                           {this->factory_ptr_->createScalar(err2print)}));

          // Reset print error
          print_error = false;
        }
      }

      // Check arguments
      for (auto &i: int2check) {
        // Safety check
        if (i >= inputs.size()) {
          print_error = true;
        }

        if (! print_error) {
          if (inputs[i].getType() != matlab::data::ArrayType::INT16 ||
              inputs[i].getType() == matlab::data::ArrayType::COMPLEX_INT16 ||
              inputs[i].getNumberOfElements() != 1) {
            print_error = true;
          }
        }


        if (print_error) {
          // Prepare string
          err2print = tools::string::print2string("Input %d must be of type int16", i + 1);

          // Show error
          this->matlab_ptr_->feval(u"error",
                                   0,
                                   std::vector<matlab::data::Array>(
                                           {this->factory_ptr_->createScalar(err2print)}));

          // Reset print error
          print_error = false;
        }
      }

      // Check arguments
      for (auto &i: str2check) {
        // Safety check
        if (i >= inputs.size()) {
          print_error = true;
        }

        if (! print_error) {
          if (inputs[i].getType() != matlab::data::ArrayType::MATLAB_STRING ||
              inputs[i].getNumberOfElements() != 1) {
            print_error = true;
          }
        }


        if (print_error) {
          // Prepare string
          err2print = tools::string::print2string("Input %d must be of type MATLAB_STRING", i + 1);

          // Show error
          this->matlab_ptr_->feval(u"error",
                                   0,
                                   std::vector<matlab::data::Array>(
                                           {this->factory_ptr_->createScalar(err2print)}));

          // Reset print error
          print_error = false;
        }
      }

      // Check number of outputs
      if (outputs.size() > 1) {
        this->matlab_ptr_->feval(u"error",
                                 0,
                                 std::vector<matlab::data::Array>({this->factory_ptr_->createScalar("Only one output is returned")}));
      }
    }

    SuperManifold *
    propagate_orbit_loads_SUPER_MANIFOLD(const std::vector<double> &t, double &nli, int &n_max, PROBLEM prob,
                                         const double *inertia = nullptr, bool simple_DA_propagation = false) {
      this->matlab_ptr_->feval(u"fprintf",
                               0,
                               std::vector<matlab::data::Array>({this->factory_ptr_->createScalar(scv0.toString())}));

      // Initial and final time and time step
      double const t0 = t[0];
      double const tf = t[1];
      double const dt = t[2];

      // Initialize integrator
      auto objIntegrator = std::make_unique<integrator>(INTEGRATOR::RK4,
                                                        simple_DA_propagation ? ALGORITHM::NONE : ALGORITHM::LOADS,
                                                        dt);

      // Build super manifold: LOADS
      auto super_manifold = new SuperManifold(nli, n_max, simple_DA_propagation ? ALGORITHM::NONE : ALGORITHM::LOADS);

      // Set beta constant in integrator
      objIntegrator->set_beta(const_cast<std::vector<double> &>(betas));

      // Initialize problem
      problems *problem;

      if (prob == PROBLEM::TWO_BODY) {
        problem = new problems(prob, 1);
      } else if (prob == PROBLEM::FREE_TORQUE_MOTION) {
        // Free Torque Motion problem
        problem = new problems(prob);

        // Create double[3][3]
        double inertia3x3[3][3];

        for (int i = 0; i < 3; i ++) {
          for (int j = 0; j < 3; j ++) {
            // Pass values
            inertia3x3[i][j] = inertia[i * 3 + j];

            // Prepare string
            auto err2print = tools::string::print2string("i: %d, j: %d --> %f\n", i, j, inertia3x3[i][j]);

            // Show error
            this->matlab_ptr_->feval(u"fprintf",
                                     0,
                                     std::vector<matlab::data::Array>(
                                             {this->factory_ptr_->createScalar(err2print)}));
          }
        }

        // Set the inertia matrix in problem object
        problem->set_inertia_matrix(inertia3x3);
      } else {
        // Throw error
      }

      // Deduce whether interruption should be made or not
      auto interruption = true;

      // Set problem ptr in the integrator
      objIntegrator->set_problem_ptr(problem);

      // Setting integrator parameters
      objIntegrator->set_integration_parameters(scv0, t0, tf, interruption);

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

    void
    initialize_initial_conditions(const std::vector<double> &ini_state, const std::vector<double> &stddev, double &ci) {

      // Initialize DACE with 6 variables
      DACE::DA::init(2, ini_state.size());

      // Initialize beta
      this->betas = std::vector<double>(stddev.size());
      for (int i = 0; i < stddev.size(); i ++)
        this->betas[i] = ci * stddev[i];

      // Initialize state DA vector
      this->scv0 = DACE::AlgebraicVector<DACE::DA>(ini_state.size());
      for (int i = 0; i < ini_state.size(); i ++)
        scv0[i] = ini_state[i] + this->betas[i] * DACE::DA(i + 1);
    }


    std::vector<std::string>
    propagate_orbit_get_final_DA(const std::vector<double> &ini_state, const std::vector<double> &stddev,
                                 const std::vector<double> &t, double &ci, double &nli, int &n_max,
                                 int &n_samples, PROBLEM prob, const double *inertia = nullptr) {
      // Initialize initial conditions
      this->initialize_initial_conditions(ini_state, stddev, ci);

      // Get super manifold
      auto super_manifold = propagate_orbit_loads_SUPER_MANIFOLD(t, nli, n_max, prob, inertia);

      // Write in blob format
      auto res = (DACE::AlgebraicVector<DACE::DA>) super_manifold->current_->front();

      // Write in stream
      //std::ofstream wout;
      //wout.open("/home/bryan/CLionProjects/ISAE/research_project/repos/VerneDA/out/test_file.blob");
//
      //wout << res;
//
      //wout.close();
      // Create result
      std::vector<std::string> res_str;
      res_str.reserve(res.size());

      // Iterate and save
      for (auto &da_str: res) {
        res_str.push_back(da_str.toString());
      }

      // Return result
      return res_str;
    }

    std::shared_ptr<dace_array>
    propagate_orbit_loads(const std::vector<double> &ini_state, const std::vector<double> &stddev,
                          const std::vector<double> &t, double &ci, double &nli, int &n_max, int &n_samples,
                          PROBLEM prob, const double *inertia = nullptr) {
      // Initialize initial conditions
      this->initialize_initial_conditions(ini_state, stddev, ci);

      // Get super manifold
      auto super_manifold = propagate_orbit_loads_SUPER_MANIFOLD(t, nli, n_max, prob, inertia);

      // Build deltas class
      auto deltas_engine = std::make_shared<delta>();

      // Set quaternion distribution stuff in case of FTMP
      if (prob == PROBLEM::FREE_TORQUE_MOTION) {
        // Set some options
        deltas_engine->set_bool_option(DELTA_GENERATOR_OPTION::ATTITUDE, true);
        deltas_engine->set_bool_option(DELTA_GENERATOR_OPTION::QUAT2EULER, true);
        deltas_engine->set_sampling_option(QUATERNION_SAMPLING::OMPL_GAUSSIAN);
        deltas_engine->set_mean_quaternion_option(scv0.extract(0, 3).cons());
      }

      // Set distribution
      if (prob == PROBLEM::FREE_TORQUE_MOTION) {
        deltas_engine->set_stddevs_q(stddev);
      } else if (prob == PROBLEM::TWO_BODY) {
        deltas_engine->set_stddevs(stddev);
      } else {
        return {};
      }

      // Compute deltas
      deltas_engine->generate_deltas(DISTRIBUTION::GAUSSIAN, n_samples);

      // Insert nominal delta, this is the center
      deltas_engine->insert_nominal(static_cast<int>(ini_state.size()));

      // Set super manifold in deltas engine
      deltas_engine->set_superManifold(super_manifold);

      // Evaluate deltas
      deltas_engine->evaluate_deltas();

      // Print summary
      // std::fprintf(stdout, "%s\n", prop_summary.c_str());
      // matlabPtr->feval(u"fprintf", 0, std::vector<matlab::data::Array>({factory.createScalar(prop_summary)}));

      // Get result
      return deltas_engine->get_eval_deltas_poly();
    }
};*/