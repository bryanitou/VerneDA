/**
 * This class will hold all the attributes and methods referent to a state of an spacecraft
 */

#include "state.h"

void state::allocate_state_values(double px, double py, double pz, double vx, double vy, double vz)
{
    // Allocate values
    this->px_ = std::make_shared<DACE::DA>(px + DACE::DA(1));
    this->py_ = std::make_shared<DACE::DA>(py + DACE::DA(2));
    this->pz_ = std::make_shared<DACE::DA>(pz + DACE::DA(3));
    this->vx_ = std::make_shared<DACE::DA>(vx + DACE::DA(4));
    this->vy_ = std::make_shared<DACE::DA>(vy + DACE::DA(5));
    this->vz_ = std::make_shared<DACE::DA>(vz + DACE::DA(6));
}

DACE::AlgebraicVector<DACE::DA> state::get_state_vector()
{
    // Safety check, check that the pointers are not nullptr
    // TODO: create function here

    // Return state vector
    DACE::AlgebraicVector<DACE::DA> res = {*this->px_,
                                           *this->py_,
                                           *this->pz_,
                                           *this->vx_,
                                           *this->vy_,
                                           *this->vz_};


    return res;
}

DACE::DA state::get_parameter_copy(POSITION position)
{
    // Go to memory place
    std::shared_ptr<DACE::DA> val_ptr;

    // Switch case
    switch (position)
    {
        case POSITION::X:
            val_ptr = this->px_;
            break;
        case POSITION::Y:
            val_ptr = this->py_;
            break;
        case POSITION::Z:
            val_ptr = this->pz_;
            break;
    }

    // Check memory place
    state::check_mem_place(val_ptr.get());

    // Make a copy and exit
    return *val_ptr;
}

DACE::DA state::get_parameter_copy(VELOCITY velocity)
{
    // Go to memory place
    std::shared_ptr<DACE::DA> val_ptr;

    // Switch case
    switch (velocity)
    {
        case VELOCITY::X:
            val_ptr = this->px_;
            break;
        case VELOCITY::Y:
            val_ptr = this->py_;
            break;
        case VELOCITY::Z:
            val_ptr = this->pz_;
            break;
    }

    // Check memory place
    state::check_mem_place(val_ptr.get());

    // Make a copy and exit
    return *val_ptr;

}

void state::check_mem_place(DACE::DA* val_ptr)
{
    // Safety check
    if(val_ptr != nullptr)
    {
        // End program here
        std::printf("ERROR: state::get_parameter_copy: value to be returned not found! (%p)", (void*)val_ptr);
    }
}