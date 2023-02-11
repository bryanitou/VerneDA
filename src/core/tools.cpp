/**
 *
 */

#include "tools.h"


void tools::io::dump_variables(DACE::DA &y, DACE::DA &x, const std::string & func_form, const std::string & var_form, const std::filesystem::path & file_path)
{
    // Get directory
    auto out_dir = file_path.parent_path();

    // Check that the output path is existing
    if (!std::filesystem::is_directory(out_dir))
    {
        // TODO: Check returned flag: true / false
        std::filesystem::create_directories(out_dir);
    }

    // Create the file stream
    std::ofstream file2write;
    file2write.open(file_path);

    // Easy print
    file2write << var_form<< std::endl << x << std::endl;
    file2write << func_form << std::endl << y;

    // Close the stream
    file2write.close();
}

void tools::io::plot_variables(std::filesystem::path & path2file, const std::string& python_executable, int span,
                           bool async, bool silent)
{
    // Ensure system() is available
    if (std::system(nullptr))
    {
        // Build command
        std::string cmd{};
        cmd = "python3";
        cmd += " ";
        cmd += python_executable;
        cmd += " ";
        cmd += "--file";
        cmd += " ";
        cmd += absolute(path2file);
        cmd += " ";
        cmd += "--span";
        cmd += " ";
        cmd += std::to_string(span);
        cmd += " ";
        cmd += "--silent";
        cmd += " ";
        cmd += silent ? "true" : "false";

        // Is it async?
        if (async)
        {
            cmd += " ";
            cmd += "&";
        }

        // Launch command
        std::system(cmd.c_str());
    }
}

std::string tools::enums::VELOCITY2str(VELOCITY velocity)
{
    // Value to be returned
    std::string result;

    // Fill the value...
    result =
            VELOCITY::X == velocity ? "X" :
            VELOCITY::Y == velocity ? "Y" :
            VELOCITY::Z == velocity ? "Z" : "UNK";

    // Check returned value
    if (result == "UNK")
    {
        printf("WARNING: Could not parse VELOCITY enum. Returning '%s'", result.c_str());
    }

    // Return found value
    return result;
}

std::string tools::enums::POSITION2str(POSITION position)
{
    // Value to be returned
    std::string result;

    // Fill the value...
    result =
            POSITION::X == position ? "X" :
            POSITION::Y == position ? "Y" :
            POSITION::Z == position ? "Z" : "UNK";

    // Check returned value
    if (result == "UNK")
    {
        printf("WARNING: Could not parse POSITION enum. Returning '%s'", result.c_str());
    }

    // Return found value
    return result;

}

std::string tools::enums::DISTRIBUTION2str(DISTRIBUTION distribution)
{
    // Value to be returned
    std::string result;

    // Fill the value...
    result =
            DISTRIBUTION::GAUSSIAN == distribution ? "GAUSSIAN" :
            DISTRIBUTION::UNIFORM == distribution ? "UNIFORM" : "UNK";

    // Check returned value
    if (result == "UNK")
    {
        printf("WARNING: Could not parse DISTRIBUTION enum. Returning '%s'", result.c_str());
    }

    // Return found value
    return result;

}