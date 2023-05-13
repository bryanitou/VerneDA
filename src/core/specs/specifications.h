/**
 * Class storing all the specifications for the run.
 */
#pragma once

// System libraries
#include <vector>

// Project libraries
#include "base/enums.h"

class specifications
{
public: // Constructors

    /**
     * Default constructor
     */
    specifications() = default;

    /**
     * Default destructor
     */
     ~specifications() = default;

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

         // Propagation set?
         bool set{false};
     };

     // Initial conditions
     struct initial_conditions
     {
         LENGTH_UNITS length_units{LENGTH_UNITS::NA};
         std::vector<double> mean{};
         std::vector<double> standard_deviation{};

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

     // Initialize them all
     algebra algebra;
     propagation propagation;
     initial_conditions initial_conditions;
     ads ads;
};