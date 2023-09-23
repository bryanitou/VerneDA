//
// Created by bryan on 23/09/23.
//

#include "FileProcessor.h"


void FileProcessor::process_files()
{
    // Post-process evolution projected in the initial or final manifold
    for (auto const & film : this->out_obj.films)
    {
        // Make frames for film
        this->make_frames(film.output_dir_frames, film.output_dir_source, film.type == FILM::INITIAL);

        // Make film with ffmpeg
        FileProcessor::make_film(film.output_dir_frames);
    }

    // Post-process python plots (final domain, MC samples, partitions)
    for (auto const & output_files : this->out_obj.wdcs)
    {
        this->make_python_plot(output_files);
    }
}

void FileProcessor::make_python_plot(const structs::output::wdc_o& output_files)
{
    // Prepare arguments
    std::unordered_map<std::string, std::string> py_args = {
            {"file",          output_files.output_deltas},
            {"plot_type",     this->pyploy_translation},
            {"metrics",       "km"},
            {"centers",       output_files.output_centers},
            {"walls",         output_files.output_walls},
            {"silent",        "false"},
            {"output_prefix", output_files.output_prefix}
    };

    // Launch command
    tools::io::plot_variables(this->pyploy_banana, py_args, false);
}


void FileProcessor::make_frames(const std::filesystem::path& output_dir, const std::filesystem::path& source_dir, bool axis_fixed)
{
    // Check directory exists
    if (!std::filesystem::is_directory(output_dir))
    {
        // TODO: Check returned flag: true / false
        std::filesystem::create_directories(output_dir);
    }

    // Auxiliary variable
    std::filesystem::path output_dir_film_frame{};

    // Launch massive plotting
    for (const auto & entry : std::filesystem::directory_iterator(source_dir))
    {
        // Name of the plot
        output_dir_film_frame = output_dir / entry.path().filename().replace_extension();

        // Set arguments
        std::unordered_map<std::string, std::string> py_args = {
                {"plot_type", pyploy_translation},
                {"metrics", "km"},
                {"walls", entry.path()},
                {"silent", "false"},
                {"output_prefix", output_dir_film_frame},
                {"output_format", "png"},
                {"legend_fixed", "true"},
                {"axis_fixed", axis_fixed ? "true" : "false"}
        };
        tools::io::plot_variables(pyploy_banana, py_args, false);
    }
}

void FileProcessor::make_film(const std::filesystem::path& frames_path)
{
    // It should look like:
    // ffmpeg -framerate 5 -pattern_type glob -i '*.png' -c:v libx264 -pix_fmt yuv420p -vf "pad=ceil(iw/2)*2:ceil(ih/2)*2" out.mp4

    // Set arguments
    std::string ffmpeg_cmd = "-y";
    ffmpeg_cmd += " ";
    ffmpeg_cmd += "-framerate";
    ffmpeg_cmd += " ";
    ffmpeg_cmd += "5";
    ffmpeg_cmd += " ";
    ffmpeg_cmd += "-pattern_type";
    ffmpeg_cmd += " ";
    ffmpeg_cmd += tools::string::print2string("glob -i '%s/*.png'", frames_path.string().c_str());
    ffmpeg_cmd += " ";
    ffmpeg_cmd += "-c:v";
    ffmpeg_cmd += " ";
    ffmpeg_cmd += "libx264";
    ffmpeg_cmd += " ";
    ffmpeg_cmd += "-pix_fmt";
    ffmpeg_cmd += " ";
    ffmpeg_cmd += "yuv420p";
    ffmpeg_cmd += " ";
    ffmpeg_cmd += "-vf";
    ffmpeg_cmd += " ";
    ffmpeg_cmd += "\"pad=ceil(iw/2)*2:ceil(ih/2)*2\"";
    ffmpeg_cmd += " ";
    ffmpeg_cmd += (frames_path / "out.mp4");


    // Launch command
    tools::io::make_film(ffmpeg_cmd, false);
}

void FileProcessor::set_wdc(const std::string& output_walls, const std::string& output_deltas,
                            const std::string& output_centers, const std::string& output_prefix)
{
    // Create struct
    structs::output::wdc_o output2add = {output_walls, output_deltas, output_centers, output_prefix};

    // Add new output
    this->out_obj.wdcs.push_back(output2add);
}

void FileProcessor::set_film(const std::string& output_dir_frames, const std::string& output_dir_source, FILM film_type)
{
    // Create struct
    structs::output::film film2add = {output_dir_frames, output_dir_source, film_type};

    // Add new output
    this->out_obj.films.push_back(film2add);
}

void FileProcessor::set_ucflags(const std::string& pyplot_translation, const std::string& pyplot_banana)
{
    // Set values
    this->pyploy_translation = pyplot_translation;
    this->pyploy_banana = pyplot_banana;

    // Set UCFLAGS set
    if (!this->pyploy_translation.empty() and !this->pyploy_banana.empty())
    {
        this->uc_flags_set = true;
    }
}
