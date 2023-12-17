/**
 * Processes files that have been already being written
 */

# pragma once

// Project libraries
#include <utility>

#include "tools/io.h"
#include "base/structs.h"

class FileProcessor
{
public: // Constructor

    /**
     * Default constructor
     */
    explicit FileProcessor(structs::out_obj out_obj) : out_obj(std::move(out_obj)) {};

    /**
     * Default destructor
     */
    ~ FileProcessor() = default;

private: // Private attributes

    // Constants that must be set by user
    std::string pyplot_type_;
    std::string pyploy_banana_;
    bool uc_flags_set{false};

    // Output object storing all info to be launched
    structs::out_obj out_obj{};

    // Set options
    std::string metrics_{};
    std::string silent_{"true"};

private: // Private methods

    /**
     * Make frames
     */
    void make_frames(const std::filesystem::path &output_dir, const std::filesystem::path &source_dir, bool axis_fixed = false);

    /**
     * Make film
     */
    static void make_film(const std::filesystem::path &frames_path);

    /**
     * Make python plots...
     * @param output_prefix [in] [std::filesystem::path]
     */
    void make_python_plot(const structs::output::wdc_o &output_files);

public: // Public setters

    /**
     * Set UCFLAGS
     * @param pyplot_type [in] [std::string]
     * @param pyplot_banana [in] [std::string]
     */
    void set_ucflags(const std::string& pyplot_type, const std::string& pyplot_banana);

    /**
     * Set one output to be processed
     * @param output_walls [in] [std::string]
     * @param output_deltas [in] [std::string]
     * @param output_centers [in] [std::string]
     * @param output_prefix [in] [std::string]
     */
    void set_wdc(const std::string& output_walls, const std::string& output_deltas, const std::string& output_centers, const std::string& output_prefix);

    /**
     * Set ont output to be processed
     * @param output_dir_frames [in] [std::string]
     * @param output_dir_source [in] [std::string]
     * @param film_type [in] [std::string]
    */
    void set_film(const std::string& output_dir_frames, const std::string& output_dir_source, FILM film_type);

    void set_metrics(LENGTH_UNITS metrics);
    void set_silent(std::string silent) { this->silent_ = std::move(silent); };

public: // Public methods

    /**
     * Process everything we have
     */
    void process_files();
};