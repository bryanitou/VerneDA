/**
 *
 */

#include "tools.h"


void tools::dump_variables(DACE::DA &y, DACE::DA &x, const std::string & func_form, const std::string & var_form, const std::filesystem::path & file_path)
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

void tools::plot_variables(std::filesystem::path & path2file, const std::string& python_executable, int span,
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