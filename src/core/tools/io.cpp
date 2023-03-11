/**
 * IO: In / Out space. Namespace dedicated to tools.
 */

#include "io.h"

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

void tools::io::dace::dump_variables(DACE::DA &func, const std::string& id,  const std::filesystem::path &file_path)
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
    file2write << id << std::endl << func << std::endl;

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
            auto exponents_str = tools::vector::int2string(exponents, " ");

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

void tools::io::dace::dump_deltas(delta* delta, const std::filesystem::path &file_path)
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

    // Get the pointer where all the evaluations are
    auto deltas_poly = delta->get_deltas_poly();

    // Write the header
    file2write << "DELTA_ID, VARIABLE, INDEX, COEFFICIENT, ORDER, EXPONENTS" << std::endl;

    // Iterate through the deltas
    for (int d = 0; d < deltas_poly->size(); d++)
    {
        // Iterate through every DA variable in this delta variation
        for (int v = 0; v < (*deltas_poly)[d].size(); v++)
        {
            // Retrieve values to print
            DACE::DA da_var = (*deltas_poly)[d][v];
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
                auto exponents_str = tools::vector::int2string(exponents, " ");

                // Prepare line to write
                auto line2write = tools::string::print2string("%i, %i, %i, %0.8f, %i, %s",
                                                              d, v, i, coef, order, exponents_str.c_str());

                // Write line
                file2write << line2write << std::endl;
            }
        }
    }

    // Close the stream
    file2write.close();

}

[[maybe_unused]] void tools::io::plot_variables(std::filesystem::path & path2file, const std::string& python_executable, int span,
                                                bool async, bool silent)
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