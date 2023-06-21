/**
 * JSON parser.
 */

#include "json/json_parser.h"

json_input json_parser::parse_input_file(const std::string& filepath)
{
    // Auxiliary variables
    std::string json_text{};
    std::string json_line{};

    // Create an empty struct to be returned
    json_input my_specs{};

    // Set name of the file
    my_specs.filepath = filepath;

    if (filepath.empty())
    {
        std::fprintf(stderr, "FATAL: File parameter has not been passed... Cannot load any configuration "
                             "to simulate.\n");
        std::exit(12);
    }

    // Create handler
    std::ifstream f;

    // Open file
    f.open(filepath);

    // Read file
    if (f.is_open())
    {
        // While we reading... set values
        while (f.good())
        {
            // Read line and assign it to 'json_text' variable
            f  >> json_line;
            json_text += json_line;
        }
    }
    else
    {
        std::fprintf(stderr, "FATAL: Couldn't open file, ensure it exists: '%s'\n", filepath.c_str());
        std::exit(11);
    }
    // TODO: Add fallbacks

    // Build parsed object
    auto my_resource_obj = RSJresource(json_text);

    // Read the length_units of the numbers
    std::string input_str = tools::string::clean_bars(my_resource_obj["input"].as_str());

    // Parse as input object
    auto input_rsj_obj = RSJresource(input_str);

    // Read input ----------------------------------------------------------------------------------
    json_parser::parse_input_section(input_rsj_obj, &my_specs);

    // Read algebra ---------------
    auto algebra_rsj_obj = json_parser::get_subsection(input_rsj_obj, json_parser::subsections::ALGEBRA);

    // TODO: Safety checks here
    my_specs.algebra.order = algebra_rsj_obj["order"].as<int>();
    my_specs.algebra.variables = algebra_rsj_obj["variables"].as<int>();
    my_specs.algebra.set = true;

    // Read propagation ---------------
    auto prop_rsj_obj = json_parser::get_subsection(input_rsj_obj, json_parser::subsections::PROPAGATION);

    // Parse propagation
    json_parser::parse_propagation_section(prop_rsj_obj, &my_specs);

    // Read initial conditions ---------------
    auto initial_conditions_rsj_obj = json_parser::get_subsection(input_rsj_obj, json_parser::subsections::INITIAL_CONDITIONS);

    // Parse initial conditions
    json_parser::parse_initial_conditions_section(initial_conditions_rsj_obj, &my_specs);

    // Read ADS/LOADS ---------------
    if (my_specs.algorithm == ALGORITHM::ADS)
    {
        // Get ADS
        auto ads_rsj_obj = json_parser::get_subsection(input_rsj_obj, json_parser::subsections::ADS);

        // Parse ADS
        json_parser::parse_ads_section(ads_rsj_obj, &my_specs);
    }
    else if (my_specs.algorithm == ALGORITHM::LOADS)
    {
        // Get LOADS
        auto loads_rsj_obj = json_parser::get_subsection(input_rsj_obj, json_parser::subsections::LOADS);

        // Parse LOADS
        json_parser::parse_loads_section(loads_rsj_obj, &my_specs);

        // Get SCALING
        auto scaling_rsj_obj = json_parser::get_subsection(input_rsj_obj, json_parser::subsections::SCALING);

        // Parse SCALING
        json_parser::parse_scaling_section(scaling_rsj_obj, &my_specs);
    }
    else
    {
        // No algorithm has been chosen
        std::fprintf(stdout, "No algorithm (ADS / LOADS) has been chosen!");
    }

    // Set beta, relying on which algorithm was used
    json_parser::set_betas(&my_specs);

    // Read output -----------
    std::string output_str = tools::string::clean_bars(my_resource_obj["output"].as_str());

    // Parse as input object
    auto output_rsj_obj = RSJresource(output_str);

    // Read the output directory where the results will be dumped
    my_specs.output_dir = tools::string::clean_bars(output_rsj_obj["directory"].as_str());

    // Check health of the inputs
    json_parser::safety_checks(&my_specs);

    // Return the object
    return my_specs;
}

// All sub-sections here
void json_parser::parse_input_section(RSJresource& rsj_obj, json_input * json_input_obj)
{
    // TODO: Not used now but will be required later
    json_input_obj->mu = rsj_obj["mu"].as<double>();

    // Parse the algorithm to be used: ADS or LOADS
    std::string algorithm_str = tools::string::clean_bars(rsj_obj["algorithm"].as_str());
    std::transform(algorithm_str.begin(), algorithm_str.end(), algorithm_str.begin(), ::tolower);
    json_input_obj->algorithm =
            algorithm_str == "ads" ? ALGORITHM::ADS :
            algorithm_str == "loads" ? ALGORITHM::LOADS :
            algorithm_str == "none" || algorithm_str == "-" || algorithm_str.empty() ? ALGORITHM::NONE : ALGORITHM::NA;

    // Safety checks
    if (json_input_obj->algorithm == ALGORITHM::NA)
    {
        // Info and exit program
        std::fprintf(stdout, "Algorithm type should be: ADS, LOADS or NONE. JSON "
                             "file: '%s'\n", json_input_obj->filepath.c_str());

        // Exit program
        std::exit(10);
    }

    // Parse the problem str
    std::string problem_str = tools::string::clean_bars(rsj_obj["problem"].as_str());
    std::transform(problem_str.begin(), problem_str.end(), problem_str.begin(), ::tolower);
    json_input_obj->problem =
            problem_str == "two_body_problem" ? PROBLEM::TWO_BODY :
            problem_str == "free_torque_motion" ? PROBLEM::FREE_TORQUE_MOTION : PROBLEM::NA;

    // Safety checks
    if (json_input_obj->problem == PROBLEM::NA)
    {
        // Info and exit program
        std::fprintf(stdout, "Problem type should be: 'two_body_problem' and 'free_torque_motion'. JSON "
                             "file: '%s'\n", json_input_obj->filepath.c_str());

        // Exit program
        std::exit(10);
    }
}

void json_parser::parse_propagation_section(RSJresource& rsj_obj, json_input * json_input_obj)
{
    json_input_obj->propagation.initial_time = rsj_obj["initial_time"].as<double>();
    json_input_obj->propagation.final_time = rsj_obj["final_time"].as<double>();
    json_input_obj->propagation.time_step = rsj_obj["time_step"].as<double>();

    auto integrator_str = tools::string::clean_bars(rsj_obj["integrator"].as_str());
    std::transform(integrator_str.begin(), integrator_str.end(), integrator_str.begin(), ::tolower);
    json_input_obj->propagation.integrator =
            integrator_str == "rk4"     ? INTEGRATOR::RK4   :
            integrator_str == "euler"   ? INTEGRATOR::EULER :
            integrator_str == "rk78"    ? INTEGRATOR::RK78  : INTEGRATOR::NA;

    json_input_obj->propagation.set = true;

    // TODO: Add safety checks here
}

void json_parser::parse_initial_conditions_section(RSJresource& rsj_obj, json_input * json_input_obj)
{
    auto initial_conditions_units_str = tools::string::clean_bars(rsj_obj["length_units"].as_str());
    json_input_obj->initial_conditions.length_units =
            initial_conditions_units_str == "km" ? LENGTH_UNITS::KILOMETERS :
            initial_conditions_units_str == "m" ? LENGTH_UNITS::METERS : LENGTH_UNITS::NA;

    json_input_obj->initial_conditions.mean = rsj_obj["mean"].as_vector<double>();
    json_input_obj->initial_conditions.standard_deviation = rsj_obj["standard_deviation"].as_vector<double>();
    json_input_obj->initial_conditions.confidence_interval = rsj_obj["confidence_interval"].as<double>();
    json_input_obj->initial_conditions.set = true;

    // If kilometers, convert to meters
    if (json_input_obj->initial_conditions.length_units == LENGTH_UNITS::KILOMETERS)
    {
        for (auto & val : json_input_obj->initial_conditions.mean)
        {
            val = val * 1000;
        }
        for (auto & val : json_input_obj->initial_conditions.standard_deviation)
        {
            val = val * 1000;
        }
    }

    // If we do have attitude problems... then parse inertia matrix
    bool attitude_problem = PROBLEM::FREE_TORQUE_MOTION == json_input_obj->problem;

    // Check whether or not to parse attitude stuff
    if (attitude_problem)
    {
        // Get the array as a RSJ object
        auto inertia_matrix_rsj_obj = rsj_obj["inertia"].as_array();

        // Iterate for every row
        for (int i = 0; i < inertia_matrix_rsj_obj.size(); i++)
        {
            // Parse row as double vector
            auto row = inertia_matrix_rsj_obj[i].as_vector<double>();

            // Print info
            std::fprintf(stdout, "Inertia matrix row '%d': [", i);

            // Iterate through every column
            for (int j = 0; j < row.size(); j++)
            {
                json_input_obj->initial_conditions.inertia[i][j] = row[j];

                // Info
                std::fprintf(stdout, " %10.2f", json_input_obj->initial_conditions.inertia[i][j]);
            }

            // Print info
            std::fprintf(stdout, "]\n");
        }

        // The standard deviation of the individual components of the tangent
        // perturbation needs to be scaled so that the expected quaternion distance
        // between the sampled state and the mean state is stdDev. The factor 2 is
        // due to the way we define distance (see also Matt Mason's lecture notes
        // on quaternions at
        // http://www.cs.cmu.edu/afs/cs/academic/class/16741-s07/www/lectures/Lecture8.pdf).
        // The 1/sqrt(3) factor is necessary because the distribution in the tangent
        // space is a 3-dimensional Gaussian, so that the *length* of a tangent
        // vector needs to be scaled by 1/sqrt(3).
        json_input_obj->initial_conditions.standard_deviation[0] = (2. * json_input_obj->initial_conditions.standard_deviation[0]) / std::sqrt(3);
        json_input_obj->initial_conditions.standard_deviation[1] = (2. * json_input_obj->initial_conditions.standard_deviation[1]) / std::sqrt(3);
        json_input_obj->initial_conditions.standard_deviation[2] = (2. * json_input_obj->initial_conditions.standard_deviation[2]) / std::sqrt(3);
    }

    // TODO: Add safety checks here
}

void json_parser::parse_ads_section(RSJresource& rsj_obj, json_input * json_input_obj)
{
    auto ads_units_str = tools::string::clean_bars(rsj_obj["length_units"].as_str());
    json_input_obj->ads.length_units = ads_units_str == "km" ?
                                LENGTH_UNITS::KILOMETERS :  ads_units_str == "m" ?
                                                            LENGTH_UNITS::METERS : LENGTH_UNITS::NA;
    json_input_obj->ads.tolerance = rsj_obj["tolerance"].as_vector<double>();
    json_input_obj->ads.max_split = rsj_obj["max_split"].as_vector<int>();
    json_input_obj->ads.set = true;

    if (json_input_obj->ads.length_units == LENGTH_UNITS::KILOMETERS)
    {
        for (auto & val : json_input_obj->ads.tolerance)
        {
            val = val * 1000;
        }
    }

    // TODO: Add safety checks here
}

void json_parser::parse_loads_section(RSJresource& rsj_obj, json_input * json_input_obj)
{
    json_input_obj->loads.nli_threshold = rsj_obj["nli_threshold"].as<double>();
    json_input_obj->loads.max_split = rsj_obj["max_split"].as_vector<int>();
    json_input_obj->loads.set = true;
}

void json_parser::parse_scaling_section(RSJresource& rsj_obj, json_input * json_input_obj)
{
    // Parce standalone doubles
    json_input_obj->scaling.length = rsj_obj["length"].as<double>();
    json_input_obj->scaling.time = rsj_obj["time"].as<double>();
    json_input_obj->scaling.speed = rsj_obj["velocity"].as<double>();

    // TODO: Add some safety checks here... have neen they properly set?

    // Scaling variables have been set
    json_input_obj->scaling.set = true;
}

// Navigation functions here
RSJresource json_parser::get_subsection(RSJresource& rsj_obj, const std::string & subsection_name)
{
    // Get the desired section as a string style
    auto desired_section_str = tools::string::clean_bars(rsj_obj[subsection_name].as_str());

    // Create object from pure plain text to 'RSJesource' object
    return {desired_section_str};
}

void json_parser::safety_checks(json_input * json_input_obj)
{
    // Should call this function when everything set
    if (json_input_obj->algorithm == ALGORITHM::LOADS)
    {
        // Check we do have scaling
        bool scaling_error = !json_input_obj->scaling.set || json_input_obj->scaling.length == 0.0 ||
                json_input_obj->scaling.time == 0.0 || json_input_obj->scaling.speed == 0.0;

        if (scaling_error)
        {
            // Info and exit program
            std::fprintf(stderr, "There was a problem when parsing the scaling section, which is needed for LOADS. "
                                 "Ensure it is not missing and scaling values are non-zero; 'length', 'time' and 'velocity'. "
                                 "JSON file: '%s'\n", json_input_obj->filepath.c_str());

            // Exit program
            std::exit(10);
        }

        // Check the loads section
        bool loads_error = !json_input_obj->loads.set ||  json_input_obj->loads.nli_threshold == 0.0 ||
                json_input_obj->loads.max_split.empty();

        if (loads_error)
        {
            // Info and exit program
            std::fprintf(stderr, "There was a problem when parsing the LOADS section. Ensure any of these"
                                 " are missing: 'nli_threshold', 'length_units' and 'max_split'. JSON file: '%s'\n", json_input_obj->filepath.c_str());

            // Exit program
            std::exit(10);
        }

        // Check order loads
        if (json_input_obj->algebra.order > 2 || json_input_obj->algebra.order == 0)
        {
            // Info
            std::fprintf(stdout, "LOADS has been configured. DA order parsed is '%d'. Nonetheless, "
                                 "it will be overriden to 2, since LOADS algorithm is designed for that.\n",
                                 json_input_obj->algebra.order);

            // Set it
            json_input_obj->algebra.order = 2;
        }
    }

    // TODO: Do ADS safety checks
}

void json_parser::set_betas(json_input * json_input_obj)
{
    // Fill the beta scaling vector
    json_input_obj->scaling.beta.reserve(json_input_obj->initial_conditions.mean.size());

    // Do it for TWO_BODY problem for now.. TODO: To be Enhanced with other problems
    if (json_input_obj->problem == PROBLEM::TWO_BODY)
    {
        // Treat relying on the algorithm
        switch (json_input_obj->algorithm)
        {
            case ALGORITHM::LOADS:
            {
                // Loop into them
                for (auto & stddev : json_input_obj->initial_conditions.standard_deviation)
                {
                    // Push back computed beta
                    json_input_obj->scaling.beta.push_back(json_input_obj->initial_conditions.confidence_interval * stddev);
                }
                break;
            }
            case ALGORITHM::ADS:
            {
                // Same as having NONE
            }
            case ALGORITHM::NONE:
            {
                // Beta is equal to stddev vector
                json_input_obj->scaling.beta = json_input_obj->initial_conditions.standard_deviation;
                break;
            }
            default:
            {
                // Should never reach here
                std::fprintf(stderr, "Something went wrong when trying to set the beta vector.");
                std::exit(111);
            }
        }
    }
}