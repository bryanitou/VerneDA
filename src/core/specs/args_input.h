/**
 * Class containing all the arguments.
 */
#pragma once

// System libraries
#include <string>

// Project libraries
#include "base/Header_Info.h"

class args_input
{
public:
    // Constructors and destructors

    /**
     * Default constructor
     */
    args_input(int argc, char** argv);


    /**
     * Default destructor
     */
    ~args_input() = default;

public:
    // Public attributes for the program run

    // Whether to log or not...
    bool silent{false};

    // Show banner?
    bool banner{false};

    // Show comilation info
    bool compilation_info{false};

    // Input file
    std::string json_filepath{};

public:
    // Methods
    /**
     * Will run the arguments as requested
     */
    void run_arguments(HeaderInfo& header_info) const;

};
