/**
 * This class will hold all the attributes and methods referent to a scv of an spacecraft
 */

#include "scv.h"

scv::scv(const DACE::AlgebraicVector<DACE::DA>& csv_DA,  bool with_da, double error)
{
    // Call to main allocator
    this->allocate_csv_DA_vector(csv_DA, with_da, error);
}

void scv::allocate_csv_DA_vector(const DACE::AlgebraicVector<DACE::DA>& csv_DA, bool with_da, double error)
{
    // Safety check it has at least 6 positions
    bool greater_size = 6 < csv_DA.size();
    bool right_size = 6 == csv_DA.size();

    // Safety check
    if (!greater_size && !right_size)
    {
        // EXIT the program for safety reasons
        std::printf("FATAL: When allocating from a DA vector, length = 6 was expected. "
                    "Actual size: '%i'. Vector should be at least of length = 6.", (int)csv_DA.size());
    }

    // Safety check
    if (greater_size)
    {
        // WARN the user that the rest will be lost,
        std::printf("WARNING: When allocating from a DA vector, length = 6 was expected. "
                    "Actual size: '%i'. Positions > 5 will be lost.\n", (int)csv_DA.size());
    }

    // Allocate variables
    for (int i = 0; i < csv_DA.size(); i++)
    {
        this->state.push_back(with_da ? csv_DA[i] + DACE::DA(i + 1) * error : csv_DA[i]);
    }
}

DACE::AlgebraicVector<DACE::DA> scv::get_state_vector_copy()
{
    // Safety check, check that the pointers are not nullptr
    // TODO: create function here
    return this->state;
}

void scv::check_mem_place(DACE::DA* val_ptr)
{
    // Safety check
    if(val_ptr != nullptr)
    {
        // End program here
        std::printf("ERROR: scv::get_parameter_copy: value to be returned not found! (%p)", (void*)val_ptr);
    }
}