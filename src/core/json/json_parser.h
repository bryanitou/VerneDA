/**
 * JSON Parser.
 */
#pragma once

// Include system libraries
#include <string>
#include <vector>
#include <cmath>

// Include JSON thirdparty parser
#include "RSJparser.tcc"

// Include project libraries
#include "tools/str.h"
#include "specs/json_input.h"

namespace json_parser
{
    namespace subsections
    {
        const std::string ALGEBRA = "algebra";
        const std::string PROPAGATION = "propagation";
        const std::string INITIAL_CONDITIONS = "initial_conditions";
        const std::string ADS = "ads";
        const std::string LOADS = "loads";
        const std::string SCALING = "scaling";
    }

    /**
     * Parses the input file
     * @param filepath std::string
     */
    json_input parse_input_file(const std::string& filepath);

    /**
     * Parse input section
     * @param rsj_obj
     * @param json_input_obj
     */
    void parse_input_section(RSJresource &rsj_obj, json_input *json_input_obj);

    void parse_propagation_section(RSJresource &rsj_obj, json_input *json_input_obj);

    void parse_initial_conditions_section(RSJresource &rsj_obj, json_input *json_input_obj);

    void parse_ads_section(RSJresource &rsj_obj, json_input *json_input_obj);

    RSJresource get_subsection(RSJresource& rsj_obj, const std::string & subsection_name);


    void safety_checks(json_input *json_input_obj);

    void parse_loads_section(RSJresource &rsj_obj, json_input *json_input_obj);

    void parse_scaling_section(RSJresource &rsj_obj, json_input *json_input_obj);

    void set_betas(json_input *json_input_obj);
};

