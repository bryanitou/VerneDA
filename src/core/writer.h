/**
 * Class in charge of writing files
 */

#pragma once

// Project libraries
# include "delta.h"
#include "tools/io.h"
#include "base/structs.h"

class writer
{
public: // Constructor

    /**
     * Constructor
     */
    writer() = default;

    /**
     * Default destructor
     */
     ~ writer() = default;

private: // Private attributes

    // Bools to whether print some files or not
    bool dump_raw_deltas{false};
    bool dump_fin_deltas{false};
    bool dump_ini_deltas{false};
    bool dump_ini_center{false};
    bool dump_fin_center{false};
    bool dump_ini_walls{false};
    bool dump_fin_walls{false};

    // Bools to whether dump frames or not
    bool dump_ini_frames{false};
    bool dump_fin_frames{false};

    // Bools to know if it has been set
    bool walls_bool_set{false};
    bool centers_bool_set{false};

private: // Private attributes

    structs::out_obj out_obj{};

public: // Setters

    /**
     * Set results to be dumped
     * @param final [in] [bool]
     * @param initial [in] [bool]
     */
    void set_dump_nominal_results(bool final = true, bool initial = false);

    /**
     * Set results to be dumped
     * @param final [in] [bool]
     * @param initial [in] [bool]
     */
    void set_dump_frames_results(bool final = false, bool initial = false);

    /**
     * Set whether to print walls or not
     * @param walls
     */
    void set_dump_walls_results(bool walls = true);

    /**
     * Set whether to print centers or not
     * @param centers
     */
    void set_dump_centers_results(bool centers = true);

public: // Getters

    /**
     * Get out object
     * @return structs::out_obj
     */
    auto get_out_obj() {return this->out_obj;}

public: // Public methods

    /**
    * Basic configuration
    */
    void basic_config();

    /**
     * Writes all the possible outputs, they have to be preset beforehand
     * @param delta [in] [delta*]
     * @param output_dir [in] [std::filesystem::path]
     */
    void write_files(delta* delta, const std::filesystem::path& output_dir);
};

