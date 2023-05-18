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

    // Read ads ---------------
    auto ads_rsj_obj = json_parser::get_subsection(input_rsj_obj, json_parser::subsections::ADS);

    // Parse ads
    json_parser::parse_ads_section(ads_rsj_obj, &my_specs);

    // Read output -----------
    std::string output_str = tools::string::clean_bars(my_resource_obj["output"].as_str());

    // Parse as input object
    auto output_rsj_obj = RSJresource(output_str);

    // Read the output directory where the results will be dumped
    my_specs.output_dir = tools::string::clean_bars(output_rsj_obj["directory"].as_str());

    // Return the object
    return my_specs;
}

// All sub-sections here
void json_parser::parse_input_section(RSJresource& rsj_obj, json_input * json_input_obj)
{
    // TODO: Not used now but will be required later
    double mu = rsj_obj["mu"].as<double>();

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
                             "file: '%s'", json_input_obj->filepath.c_str());

        // Exit program
        std::exit(10);
    }
}

void json_parser::parse_propagation_section(RSJresource& rsj_obj, json_input * json_input_obj)
{
    json_input_obj->propagation.initial_time = rsj_obj["initial_time"].as<double>();
    json_input_obj->propagation.final_time = rsj_obj["final_time"].as<double>();
    json_input_obj->propagation.time_step = rsj_obj["time_step"].as<double>();

    auto integrator_str = tools::string::clean_bars(rsj_obj["interator"].as_str());
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

// Navigation functions here
RSJresource json_parser::get_subsection(RSJresource& rsj_obj, const std::string & subsection_name)
{
    // Get the desired section as a string style
    auto desired_section_str = tools::string::clean_bars(rsj_obj[subsection_name].as_str());

    // Create object from pure plain text to 'RSJesource' object
    return {desired_section_str};
}