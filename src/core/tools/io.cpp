/**
 * IO: In / Out space. Namespace dedicated to tools.
 */

#include "io.h"
#include <vector>

void tools::io::dace::dump_eval_deltas(delta* delta, const std::filesystem::path &file_path, EVAL_TYPE eval_type)
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

    // Get problem statement
    auto problem_type = delta->get_SuperManifold()->get_manifold_fin()->get_integrator_ptr()->get_problem_ptr()->get_type();

    // Iterate through the deltas
    if (eval_type == EVAL_TYPE::FINAL_WALLS || eval_type == EVAL_TYPE::INITIAL_WALLS)
    {
        // Compute wall points
        auto patches = eval_type == EVAL_TYPE::FINAL_WALLS ?
                       (
                               problem_type == PROBLEM::FREE_TORQUE_MOTION ?
                               delta->get_SuperManifold()->get_att6dof_fin()->wallsPointEvaluationManifold() :
                               delta->get_SuperManifold()->get_manifold_fin()->wallsPointEvaluationManifold()
                       )
                       :
                       (
                               problem_type == PROBLEM::FREE_TORQUE_MOTION ?
                               delta->get_SuperManifold()->get_att6dof_ini()->wallsPointEvaluationManifold() :
                               delta->get_SuperManifold()->get_manifold_ini()->wallsPointEvaluationManifold()
                               )
                       ;

        // Print them all
        tools::io::dace::print_each_patch_wall(patches, file2write, eval_type);
    }
    else
    {
        // Get the pointer where all the evaluations are
        auto deltas_poly = eval_type == EVAL_TYPE::FINAL_CENTER ?
                           (
                                   problem_type == PROBLEM::FREE_TORQUE_MOTION ?
                                   delta->get_SuperManifold()->get_att6dof_fin()->centerPointEvaluationManifold() :
                                   delta->get_SuperManifold()->get_manifold_fin()->centerPointEvaluationManifold()
                           )
                           :
                           eval_type == EVAL_TYPE::INITIAL_CENTER ?
                           (
                                   problem_type == PROBLEM::FREE_TORQUE_MOTION ?
                                   delta->get_SuperManifold()->get_att6dof_ini()->centerPointEvaluationManifold() :
                                   delta->get_SuperManifold()->get_manifold_ini()->centerPointEvaluationManifold()
                            ):
                eval_type == EVAL_TYPE::FINAL_DELTA ?
                *delta->get_eval_deltas_poly() : *delta->get_non_eval_deltas_poly();

        // Print the evaluated points
        tools::io::dace::print_each_delta(deltas_poly, file2write, eval_type);
    }


    // Close the stream
    file2write.close();

}

void tools::io::dace::dump_splitting_history(delta *delta, const std::filesystem::path &file_path)
{
    // Get current manifold
    auto current_manifold = delta->get_SuperManifold()->current_;

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

    // String containing the history
    std::string history{};
    std::string times{};
    std::string nlis{};
    std::string line2write{};

    // Write the header
    file2write << "PATCH_ID, HISTORY, SPLIT_NLI, BIRTH_TIME, SPLITTING_TIME" << std::endl;

    std::vector<double> nli_values; // Vector to store patch.nli values
    std::vector<double> time_values; // Vector to store time values

    int i = 0;
    for (auto & patch : *current_manifold)
    {
        // Safety check it is not empty
        if (patch.history_is_empty())
        {
            continue;
        }

        // Get the vector
        history = tools::vector::num2string(patch.get_history_int(), ", ", "%3d");
        times = tools::vector::num2string(patch.get_times_doubles(), ", ", "%3.16f");
        nlis = tools::vector::num2string(patch.get_nlis_doubles(), ", ", "%3.16f");

        // Store nli value in the vector
        nli_values.push_back(patch.nli);

        line2write = tools::string::print2string( "%3d, %s, %2.16f, %2.16f, %s, %s",
                                                  i, history.c_str(), patch.nli, patch.t_split_, times.c_str(), nlis.c_str());

        // Write line
        file2write << line2write << std::endl;

        i++;
    }

    // Close file
    file2write.close();

    // Save nli values to a separate CSV file
    std::ofstream nli_file("nli_values.csv");
    for (double nli : nli_values) {
        nli_file << nli << std::endl;
    }
    nli_file.close();
}

void tools::io::dace::print_manifold_evolution(delta* delta, const std::filesystem::path &dir_path, EVAL_TYPE eval_type)
{
    // Auxiliary variable
    std::filesystem::path file_path{};

    // From each manifold, print centers, walls and deltas if available
    auto domain_record =  eval_type == EVAL_TYPE::INITIAL_WALLS ? delta->get_SuperManifold()->get_manifold_fin()->get_ini_domain_record() :
                          delta->get_SuperManifold()->get_manifold_fin()->get_fin_domain_record();

    // Check directory exists
    if (!std::filesystem::is_directory(dir_path))
    {
        // TODO: Check returned flag: true / false
        std::filesystem::create_directories(dir_path);
    }

    // Iterate through every domain record
    for (unsigned int i = 0; i < domain_record->size(); i++)
    {
        // Evaluate walls
        auto intermediate_manifold_patches =
                eval_type == EVAL_TYPE::INITIAL_WALLS ? domain_record->at(i)->get_initial_split_domain()->wallsPointEvaluationManifold() :
                domain_record->at(i)->wallsPointEvaluationManifold() ;

        // Build file path from dir
        file_path = dir_path / tools::string::print2string("%s_eval_walls-%06d.walls", eval_type == EVAL_TYPE::INITIAL_WALLS ? "ini" : "fin", i);

        // Create the file stream
        std::ofstream file2write;
        file2write.open(file_path);

        // Print them all
        tools::io::dace::print_each_patch_wall(intermediate_manifold_patches, file2write, eval_type);

        // Close stream
        file2write.close();

        // Clean name
        file_path.clear();
    }
}

// TODO: Try to merge this funciton with the one above since they are almost the same
void tools::io::dace::dump_non_eval_deltas(delta* delta, const std::filesystem::path &file_path)
{
    // Auxiliary variables
    bool monomial_masked;

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
    auto non_eval_deltas_poly = delta->get_non_eval_deltas_poly();

    // Write the header
    file2write << "DELTA_ID, VARIABLE, INDEX, COEFFICIENT, ORDER, EXPONENTS" << std::endl;

    // Iterate through the deltas
    for (int d = 0; d < non_eval_deltas_poly->size(); d++)
    {
        // Retrieve scv
        auto scv = (*non_eval_deltas_poly)[d];

        // Iterate through every SCV variable in this delta variation
        for (int v = 0; v < scv.size(); v++)
        {
            // Retrieve values to print
            auto da_var = scv[v];
            int n_da_var = static_cast<int>(0);

            // Should we mask?
            // TODO: revise this masking... not very clear why this masking is needed
            monomial_masked = n_da_var == 0;

            // Print each monomial
            tools::io::dace::print_each_monomial(file2write, da_var, n_da_var, monomial_masked, {d, v});
        }
    }

    // Close the stream
    file2write.close();

}
