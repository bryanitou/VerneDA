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