/**
 * This class will hold all the attributes and methods referent to a scv of an spacecraft
 */

#pragma once

// Include system libraries
# include "iostream"
# include "memory"

// Project libraries
# include "base/enums.h"

// DACE libraries
#include "dace/dace.h"

class scv{

public: // Constructor
    explicit scv(double px, double py, double pz, double vx, double vy, double vz, double error = 100.0, bool with_da = true);
    explicit scv(const DACE::AlgebraicVector<DACE::DA>& csv_DA);
    ~scv() = default;

public:
    // Getters
    DACE::AlgebraicVector<DACE::DA> get_state_vector_copy();

public:
    // Setters
    void set_state_value(const DACE::DA& val, POSITION position);
    void set_state_value(const DACE::DA& val, VELOCITY velocity);

    // Getters
    DACE::DA get_state_value_copy(POSITION position);
    DACE::DA get_state_value_copy(VELOCITY velocity);

private:
    // Initialize values without any location in memory... so we can track them
    std::shared_ptr<DACE::DA> px_ = nullptr;
    std::shared_ptr<DACE::DA> py_ = nullptr;
    std::shared_ptr<DACE::DA> pz_ = nullptr;
    std::shared_ptr<DACE::DA> vx_ = nullptr;
    std::shared_ptr<DACE::DA> vy_ = nullptr;
    std::shared_ptr<DACE::DA> vz_ = nullptr;

private:
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
    void allocate_scv_values(double px, double py, double pz, double vx, double vy, double vz,
                             double error, bool with_da);

    /**
     * Main allocator for already built DA vector
     * @param csv_DA [DACE::AlgebraicVector<DACE::DA>]
     */
    void allocate_csv_DA_vector(const DACE::AlgebraicVector<DACE::DA> &csv_DA);

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
