/**
 * This class will hold all the attributes and methods referent to a state of an spacecraft
 */

#pragma once

// Include system libraries
# include "iostream"
# include "memory"

// Project libraries
#include "enums.h"

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
    double get_parameter_copy(POSITION position);
    double get_parameter_copy(VELOCITY velocity);


};
