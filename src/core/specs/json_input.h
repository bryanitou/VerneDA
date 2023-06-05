/**
 * Class storing all the json_input for the run.
 */
#pragma once

// System libraries
#include <vector>
#include <string>

// Project libraries
#include "base/enums.h"

class json_input
{
public: // Constructors

    /**
     * Default constructor
     */
    json_input() = default;

    /**
     * Default destructor
     */
     ~json_input() = default;

public:// Attributes

    // Specifications for the execution attributes
    struct algebra
    {
         int order{};
         int variables{};

         // Algebra set?
         bool set{false};
    };

     // Specifications for the propagation
     struct propagation
     {
         double initial_time{};
         double final_time{};
         double time_step{};
         INTEGRATOR integrator{INTEGRATOR::NA};

         // Propagation set?
         bool set{false};
     };

     // Initial conditions
     struct initial_conditions
     {
         LENGTH_UNITS length_units{LENGTH_UNITS::NA};
         std::vector<double> mean{};
         std::vector<double> standard_deviation{};
         double confidence_interval{};

         // If we have attitude...
         double inertia[3][3]{};

         // Initial conditions set?
         bool set{false};
     };

     // ADS config
     struct ads
     {
         LENGTH_UNITS length_units{LENGTH_UNITS::NA};
         std::vector<double> tolerance{};
         std::vector<int> max_split{};

         // ADS config set?
         bool set{false};
     };

     // LOADS config
     struct loads
     {
         std::vector<int> max_split{};

         // NLI threshold
         double nli_threshold{};

         // LOADS config set?
         bool set{false};
     };

     // Scaling
     struct scaling
     {
         double length{};
         double time{};
         double speed{};
         std::vector<double> beta;

         // LOADS config set?
         bool set{false};
     };

     // Initialize them all
     algebra algebra;
     propagation propagation;
     initial_conditions initial_conditions;
     ads ads;
     loads loads;
     scaling scaling;

     // Auxiliary for this class attributes
     std::string filepath;

     // Single attributes
     std::string output_dir{};
     PROBLEM problem{PROBLEM::NA};
     ALGORITHM algorithm{ALGORITHM::NA};
};