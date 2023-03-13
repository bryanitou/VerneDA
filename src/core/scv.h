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


    /**
     * State control vector class constructor from Algebraic Vector.
     * @param csv_DA [in] [DACE::AlgebraicVector<DACE::DA>]
     */
    explicit scv(const DACE::AlgebraicVector<DACE::DA>& csv_DA, bool with_da = false, double error = 1.0);

    /**
     * Class default destructor.
     */
    ~scv() = default;

public:
    // Getters
    DACE::AlgebraicVector<DACE::DA> get_state_vector_copy();

private:
    // Initialize values without any location in memory... so we can track them
    std::vector<DACE::DA> state{};

private:
    // Allocators

    /**
     * Main allocator for already built DA vector
     * @param csv_DA [DACE::AlgebraicVector<DACE::DA>]
     */
    void allocate_csv_DA_vector(const DACE::AlgebraicVector<DACE::DA> &csv_DA,  bool with_da, double error);


private:
    // Memory checks and errors
    static void check_mem_place(DACE::DA* val_ptr);
};
