/**
 * This class will hold all the attributes and methods referent to a state of an spacecraft
 */

#pragma once

// Include system libraries
# include "iostream"
# include "memory"

// Project libraries
# include "enums.h"
# include "enums_conversion.h"

class state{

public: // Constructor
    state() = default;

    ~state() = default;

private:
    // Initialize values without any location in memory... so we can track them
    std::shared_ptr<double> px_ = nullptr;
    std::shared_ptr<double> py_ = nullptr;
    std::shared_ptr<double> pz_ = nullptr;
    std::shared_ptr<double> vx_ = nullptr;
    std::shared_ptr<double> vy_ = nullptr;
    std::shared_ptr<double> vz_ = nullptr;

private:
    // Allocators
    void allocate_state_values(double px, double py, double pz, double vx, double vy, double vz);

private:
    // Access values
    /**
     * Get copy of the parameter chosen: position:
     * @param position
     * @return double
     */
    double get_parameter_copy(POSITION position);

    /**
     * Get copy of the parameter chosen: velocity.
     * @param velocity
     * @return double
     */
    double get_parameter_copy(VELOCITY velocity);


private:
    // Memory checks and errors
    static void check_mem_place(double* val_ptr);
};
