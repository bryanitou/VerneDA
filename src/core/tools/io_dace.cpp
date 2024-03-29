//
// Created by bryan on 09/11/23.
//

#include "io_dace.h"


[[maybe_unused]] void tools::io::dace::dump_variables(DACE::DA &y, DACE::DA &x, const std::string & func_form,
                                                      const std::string & var_form, const std::filesystem::path & file_path)
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
    file2write << var_form << std::endl << x << std::endl;
    file2write << func_form << std::endl << y;

    // Close the stream
    file2write.close();
}

/*
void tools::io::write2stream_DA_parameters(std::iostream * iostream, DACE::DA da_var)
{
    // Get amount of indexes
    int n_da_var = static_cast<int>(da_var.size());

    // Iterate through them
    for (int i = 1; i <= n_da_var; i++)
    {
        // Monomial
        auto mono = da_var.getMonomial(i);

        // Coef
        auto coef = mono.m_coeff;

        // Order
        auto order = mono.order();

        // String
        auto exponents = mono.m_jj;
        auto exponents_str = tools::vector::int2string(exponents, " ");

        // Prepare line to write
        auto line2write = tools::string::print2string("%i, %i, %i, %0.8f, %i, %s",
                                                      d, v, i, coef, order, exponents_str.c_str());

        // Write line
        file2write << line2write << std::endl;
    }
}*/ //TODO: Finish this

void tools::io::dace::dump_algebraic_vector(const DACE::AlgebraicVector<DACE::DA>& DA_v, const std::filesystem::path &file_path)
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

    // Write the header
    file2write << "VARIABLE, INDEX, COEFFICIENT, ORDER, EXPONENTS" << std::endl;


    // Get the size of the vector
    int n_DA_v = static_cast<int>(DA_v.size());

    for (int v = 0; v < n_DA_v; v++)
    {
        // Retrieve values to print
        const DACE::DA& da_var = DA_v[v];
        int n_da_var = static_cast<int>(da_var.size());

        for (int i = 1; i <= n_da_var; i++)
        {
            // Monomial
            auto mono = da_var.getMonomial(i);

            // Coef
            auto coef = mono.m_coeff;

            // Order
            auto order = mono.order();

            // String
            auto exponents = mono.m_jj;
            auto exponents_str = tools::vector::num2string<unsigned int>(exponents, " ");

            // Prepare line to write
            auto line2write = tools::string::print2string("%i, %i, %0.8f, %i, %s",
                                                          v, i, coef, order, exponents_str.c_str());

            // Write line
            file2write << line2write << std::endl;
        }
    }

    // Close the stream
    file2write.close();

}


void tools::io::dace::print_each_patch_wall(std::vector<std::vector<DACE::AlgebraicVector<double>>> patches, std::ofstream & file2write, EVAL_TYPE eval_type, bool print_header)
{
    // Auxiliary variable
    bool monomial_masked;

    // Print header if required
    if (print_header)
    {
        // Write the header
        file2write <<  "PATCH_ID, POINT_ID, VARIABLE, INDEX, COEFFICIENT, ORDER, EXPONENTS" << std::endl;
    }

    // Print them all
    for (int p = 0; p < patches.size(); p++)
    {
        for (int wp = 0; wp < patches[p].size(); wp++)
        {
            // Iterate through every DA variable in this delta variation
            for (int v = 0; v < patches[p][wp].size(); v++)
            {
                // Retrieve values to print
                DACE::DA da_var = patches[p][wp][v];
                int n_da_var = static_cast<int>(da_var.size());

                // Should we mask?
                // TODO: revise this masking... not very clear why this masking is needed
                monomial_masked = n_da_var == 0;

                // Print each monomial
                tools::io::dace::print_each_monomial(file2write, da_var, n_da_var, monomial_masked, {p, wp, v}, eval_type);
            }
        }
    }
}


void tools::io::dace::print_each_delta(std::vector<DACE::AlgebraicVector<double>> deltas_poly, std::ofstream & file2write, EVAL_TYPE eval_type, bool print_header)
{
    // Auxiliary variable
    bool monomial_masked;

    // Print header if required
    if (print_header)
    {
        // Write the header
        file2write << "DELTA_ID, VARIABLE, INDEX, COEFFICIENT, ORDER, EXPONENTS" << std::endl;
    }

    for (int d = 0; d < deltas_poly.size(); d++)
    {
        // Iterate through every DA variable in this delta variation
        for (int v = 0; v < deltas_poly[d].size(); v++)
        {
            // Retrieve values to print
            DACE::DA da_var = deltas_poly[d][v];
            int n_da_var = static_cast<int>(da_var.size());

            // Should we mask?
            // TODO: revise this masking... not very clear why this masking is needed
            monomial_masked = n_da_var == 0;

            // Print each monomial
            tools::io::dace::print_each_monomial(file2write, da_var, n_da_var, monomial_masked, {d, v}, eval_type);
        }
    }
}

void tools::io::dace::print_each_monomial(std::ofstream &file2write, const DACE::DA& da_var, bool n_da_var, bool monomial_masked, std::vector<int> idx, EVAL_TYPE eval_type)
{
    for (int i = monomial_masked ? 0 : 1; i <= n_da_var; i++)
    {
        // Monomial
        auto mono = da_var.getMonomial(i);

        // Coef
        auto coef = mono.m_coeff;

        // Order
        auto order = mono.order();

        // String
        auto exponents = mono.m_jj;
        auto exponents_str = tools::vector::num2string<unsigned int>(exponents, " ");

        // Prepare indexes
        auto indexes2write = tools::string::print2string("%i, %i", idx[0], idx[1]);

        // Add one more indexes if walls are to be dumped
        indexes2write += EVAL_TYPE::FINAL_WALLS == eval_type || eval_type == EVAL_TYPE::INITIAL_WALLS ? tools::string::print2string(" ,%i", idx[2]) : "";

        // Prepare line to write
        auto line2write = tools::string::print2string(" ,%i, %0.8f, %i, %s",
                                                      monomial_masked ? i + 1 : i,
                                                      coef, order, exponents_str.c_str());

        // Final line to write
        auto final2write = indexes2write + line2write;

        // Write line
        file2write << final2write << std::endl;
    }

    // Check if skipped
    if (monomial_masked)
    {
        // Convert to string
        auto str2print = tools::vector::num2string(idx, ", ");

        // Info
        std::printf("INFO: Delta/Wall: '%s', was masked due to zeros.\n", str2print.c_str());
    }
}


[[maybe_unused]] void tools::io::plot_variables(const std::string& python_executable,
                                                const std::unordered_map<std::string, std::string>& args,
                                                bool async)
{
    // Ensure system() is available
    if (std::system(nullptr))
    {
        // TODO: remove such amount of inputs and use a non-ordered map or something like this
        //  this way it will be a more general function
        // Build command
        std::string cmd{};
        cmd = "python3";
        cmd += " ";
        cmd += python_executable;
        cmd += " ";

        // Now, write all the arguments from the map
        for(const auto& [arg, value] : args)
        {
            if (!value.empty())
            {
                cmd += "--";
                cmd += arg;
                cmd += " ";
                cmd += value;
                cmd += " ";
            }
        }

        // Is it an asynchronous process?
        if (async)
        {
            cmd += "&";
            cmd += " ";
        }

        // Info
        std::fprintf(stdout, "INFO: Launching command: %s\n", cmd.c_str());

        // Launch command
        std::system(cmd.c_str());
    }
}

void tools::io::make_film(const std::string& args_str, bool async)
{
    // Ensure system() is available
    if (std::system(nullptr))
    {
        // TODO: remove such amount of inputs and use a non-ordered map or something like this
        //  this way it will be a more general function
        // Build command
        std::string cmd{};
        cmd = "ffmpeg";
        cmd += " ";
        cmd += args_str;

        // Is it an asynchronous process?
        if (async)
        {
            cmd += "&";
            cmd += " ";
        }

        // Info
        std::fprintf(stdout, "INFO: Launching command: %s\n", cmd.c_str());

        // Launch command
        std::system(cmd.c_str());
    }
}