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
    // TODO: Add fallbacks

    // Build parsed object
    auto my_resource_obj = RSJresource(json_text);

    // Read the length_units of the numbers
    std::string input_str = tools::string::clean_bars(my_resource_obj["input"].as_str());

    // Parse as input object
    auto input_rsj_obj = RSJresource(input_str);

    // Read input ----------------------------------------------------------------------------------
    // In input, read mu
    double mu = input_rsj_obj["mu"].as<double>();

    // Read algebra ---------------
    auto algebra_str = tools::string::clean_bars(input_rsj_obj["algebra"].as_str());
    auto algebra_rsj_obj = RSJresource(algebra_str);

    my_specs.algebra.order = algebra_rsj_obj["order"].as<int>();
    my_specs.algebra.variables = algebra_rsj_obj["variables"].as<int>();
    my_specs.algebra.set = true;

    // Read propagation ---------------
    auto prop_str = tools::string::clean_bars(input_rsj_obj["propagation"].as_str());
    auto prop_rsj_obj = RSJresource(prop_str);

    my_specs.propagation.initial_time = prop_rsj_obj["initial_time"].as<double>();
    my_specs.propagation.final_time = prop_rsj_obj["final_time"].as<double>();
    my_specs.propagation.time_step = prop_rsj_obj["time_step"].as<double>();

    auto integrator_str = tools::string::clean_bars(prop_rsj_obj["interator"].as_str());
    std::transform(integrator_str.begin(), integrator_str.end(), integrator_str.begin(), ::tolower);
    my_specs.propagation.integrator =
            integrator_str == "rk4"     ? INTEGRATOR::RK4   :
            integrator_str == "euler"   ? INTEGRATOR::EULER :
            integrator_str == "rk78"    ? INTEGRATOR::RK78  : INTEGRATOR::NA;

    my_specs.propagation.set = true;

    // Read initial conditions ---------------
    auto initial_conditions_str = tools::string::clean_bars(input_rsj_obj["initial_conditions"].as_str());
    auto initial_conditions_rsj_obj = RSJresource(initial_conditions_str);


    auto initial_conditions_units_str = tools::string::clean_bars(initial_conditions_rsj_obj["length_units"].as_str());
    my_specs.initial_conditions.length_units = initial_conditions_units_str == "km" ?
                                               LENGTH_UNITS::KILOMETERS :  initial_conditions_units_str == "m" ?
            LENGTH_UNITS::METERS : LENGTH_UNITS::NA;
    my_specs.initial_conditions.mean = initial_conditions_rsj_obj["mean"].as_vector<double>();
    my_specs.initial_conditions.standard_deviation = initial_conditions_rsj_obj["standard_deviation"].as_vector<double>();
    my_specs.initial_conditions.set = true;

    // If kilometers, convert to meters
    if (my_specs.initial_conditions.length_units == LENGTH_UNITS::KILOMETERS)
    {
        for (auto & val : my_specs.initial_conditions.mean)
        {
            val = val * 1000;
        }
        for (auto & val : my_specs.initial_conditions.standard_deviation)
        {
            val = val * 1000;
        }
    }

    // Read ads ---------------
    auto ads_str = tools::string::clean_bars(input_rsj_obj["ads"].as_str());
    auto ads_rsj_obj = RSJresource(ads_str);

    auto ads_units_str = tools::string::clean_bars(ads_rsj_obj["length_units"].as_str());
    my_specs.ads.length_units = ads_units_str == "km" ?
                                LENGTH_UNITS::KILOMETERS :  ads_units_str == "m" ?
                                LENGTH_UNITS::METERS : LENGTH_UNITS::NA;
    my_specs.ads.tolerance = ads_rsj_obj["tolerance"].as_vector<double>();
    my_specs.ads.max_split = ads_rsj_obj["max_split"].as_vector<int>();
    my_specs.ads.set = true;

    if (my_specs.ads.length_units == LENGTH_UNITS::KILOMETERS)
    {
        for (auto & val : my_specs.ads.tolerance)
        {
            val = val * 1000;
        }
    }

    // Read output -----------
    std::string output_str = tools::string::clean_bars(my_resource_obj["output"].as_str());

    // Parse as input object
    auto output_rsj_obj = RSJresource(input_str);

    // Read the output directory where the results will be dumped
    my_specs.output_dir = tools::string::clean_bars(output_rsj_obj["directory"].as_str());

    // Return the object
    return my_specs;
}