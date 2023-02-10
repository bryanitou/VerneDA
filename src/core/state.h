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

// DACE libraries
#include "dace/dace.h"

class state{

public: // Constructor
    state() = default;

    ~state() = default;

private:
    // Initialize values without any location in memory... so we can track them
    std::shared_ptr<DACE::DA> px_ = nullptr;
    std::shared_ptr<DACE::DA> py_ = nullptr;
    std::shared_ptr<DACE::DA> pz_ = nullptr;
    std::shared_ptr<DACE::DA> vx_ = nullptr;
    std::shared_ptr<DACE::DA> vy_ = nullptr;
    std::shared_ptr<DACE::DA> vz_ = nullptr;

public:
    // Allocators
    /**
     * Main allocator of position and velocity
     * @param px [double]
     * @param py [double]
     * @param pz [double]
     * @param vx [double]
     * @param vy [double]
     * @param vz [double]
     */
    void allocate_state_values(double px, double py, double pz, double vx, double vy, double vz);

public:
    // Getters
    DACE::AlgebraicVector<DACE::DA> get_state_vector();

private:
    // Access values
    /**
     * Get copy of the parameter chosen: position:
     * @param position
     * @return double
     */
    DACE::DA get_parameter_copy(POSITION position);

    /**
     * Get copy of the parameter chosen: velocity.
     * @param velocity
     * @return double
     */
    DACE::DA get_parameter_copy(VELOCITY velocity);


private:
    // Memory checks and errors
    static void check_mem_place(DACE::DA* val_ptr);
};
