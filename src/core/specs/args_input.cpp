/**
 * Class containing all the arguments.
 */
#include "args_input.h"

args_input::args_input(int argc, char **argv)
{
    // Process command-line arguments
    for (int i = 1; i < argc; i++)
    {
        // Get current argument
        std::string argv_str = argv[i];

        // Detect special cmd arguments
        if (argv_str == "--version") // Show compilation info
        {
            this->compilation_info = true;
        }
        else if (argv_str == "--banner") // Show full banner
        {
            this->banner = true;
        }
        else if (argv_str == "--silent") // Silent operation
        {
            this->silent = true;
            continue;
        }
        else if (argv_str == "--config") // Input: config file
        {
            this->json_filepath = argv[i + 1];
            continue;
        }
        else // This is a data input; it will be processed along its previous tag
        {
            continue;
        }
    }
}


void args_input::run_arguments(HeaderInfo& header_info) const
{
    // Auxiliary variable
    bool should_exit{false};

    if (this->banner)
    {
        header_info.print_header_info();
        should_exit = true;
    }
    else if (this->compilation_info)
    {
        header_info.print_compilation_info();
        should_exit = true;
    }

    if (should_exit)
    {
        std::exit(0);
    }
}