/**
 * Class in charge of writing files
 */

#include "writer.h"

writer::writer()
{
    // Configure
    basic_config();
}

void writer::write_files(delta* delta, const std::filesystem::path& output_dir)
{
    // Debugging files
    std::filesystem::path output_debug_splitting_history = output_dir / "splitting_history.txt";
    tools::io::dace::dump_splitting_history(delta, output_debug_splitting_history);

    // Form objects
    structs::output::wdc_o wdc_ini{};
    structs::output::wdc_o wdc_fin{};
    structs::output::film film_ini{};
    structs::output::film film_fin{};

    // Dump evaluated deltas
    if (this->dump_fin_deltas)
    {
        wdc_fin.output_deltas =  output_dir / "eval_deltas_fin.dd";
        tools::io::dace::dump_eval_deltas(delta, wdc_fin.output_deltas, EVAL_TYPE::FINAL_DELTA);
    }
    if (this->dump_ini_deltas)
    {
        wdc_ini.output_deltas  =  output_dir / "eval_deltas_ini.dd";
        tools::io::dace::dump_eval_deltas(delta, wdc_ini.output_deltas, EVAL_TYPE::INITIAL_DELTA);
    }

    // Dump eval points at the walls
    if (this->dump_fin_walls)
    {
        wdc_fin.output_walls = output_dir / "eval_walls_fin.walls";
        tools::io::dace::dump_eval_deltas(delta, wdc_fin.output_walls, EVAL_TYPE::FINAL_WALLS);
    }
    if (this->dump_ini_walls)
    {
        wdc_ini.output_walls = output_dir / "eval_walls_ini.walls";
        tools::io::dace::dump_eval_deltas(delta, wdc_ini.output_walls , EVAL_TYPE::INITIAL_WALLS);
    }

    // Dump eval points at the center
    if (this->dump_fin_center)
    {
        wdc_fin.output_centers  = output_dir / "eval_centers_fin.dd";
        tools::io::dace::dump_eval_deltas(delta, wdc_fin.output_centers, EVAL_TYPE::FINAL_CENTER);
    }
    if (this->dump_ini_center)
    {
        wdc_ini.output_centers = output_dir / "eval_centers_ini.dd";
        tools::io::dace::dump_eval_deltas(delta, wdc_ini.output_centers, EVAL_TYPE::INITIAL_CENTER);
    }

    // Extra debugging stuff
    if (this->dump_fin_frames || this->dump_ini_frames)
    {
        std::filesystem::path output_dir_film = output_dir / "film";
        std::filesystem::path output_dir_film_source = output_dir_film / "source";
        std::filesystem::path output_dir_film_frames = output_dir_film / "frames";
        if (this->dump_fin_frames)
        {
            film_fin.output_dir_frames = output_dir_film_frames / "fin";
            film_fin.output_dir_source = output_dir_film_source / "fin";
            film_fin.type = FILM::FINAL;
            tools::io::dace::print_manifold_evolution(delta, film_fin.output_dir_source, EVAL_TYPE::FINAL_WALLS);

            this->out_obj.films.emplace_back(film_fin);
        }
        if (this->dump_ini_frames)
        {
            film_ini.output_dir_frames = output_dir_film_frames / "ini";
            film_ini.output_dir_source = output_dir_film_source / "ini";
            film_ini.type = FILM::INITIAL;
            tools::io::dace::print_manifold_evolution(delta, film_ini.output_dir_source, EVAL_TYPE::INITIAL_WALLS);
            this->out_obj.films.emplace_back(film_ini);
        }
    }

    // Push back if necessary
    if (this-> dump_ini_deltas || this->dump_ini_walls || this->dump_ini_center)
    {
        wdc_ini.output_prefix = output_dir / "projection_ini";
        this->out_obj.wdcs.emplace_back(wdc_ini);
    }
    if (this-> dump_fin_deltas || this->dump_fin_walls || this->dump_fin_center)
    {
        wdc_fin.output_prefix = output_dir / "projection_fin";
        this->out_obj.wdcs.emplace_back(wdc_fin);
    }
}

void writer::set_dump_nominal_results(bool final, bool initial)
{
    // Set bools
    if (!this->walls_bool_set)
    {
        this->dump_fin_walls = final;
        this->dump_ini_walls = initial;
        this->walls_bool_set = true;
    }
    if(!this->centers_bool_set)
    {
        this->dump_fin_center = final;
        this->dump_ini_center = initial;
        this->centers_bool_set = true;
    }

    // Set final / initial deltas
    this->dump_fin_deltas = final;
    this->dump_ini_deltas = initial;

    // Usually we will always print
    this->dump_raw_deltas = true;
}

void writer::set_dump_walls_results(bool walls)
{
    // Set bools
    this->dump_fin_walls = walls;
    this->dump_ini_walls = walls;
    this->walls_bool_set = true;

}

void writer::set_dump_centers_results(bool centers)
{
    // Set bools
    this->dump_fin_center = centers;
    this->dump_ini_center = centers;
    this->centers_bool_set = true;
}

void writer::set_dump_frames_results(bool final, bool initial)
{
    // Set bools
    this->dump_fin_frames = final;
    this->dump_ini_frames = initial;
}

void writer::basic_config()
{
    // Set basic configuration
    this->set_dump_nominal_results();
    this->set_dump_frames_results();
}
