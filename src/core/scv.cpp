/**
 * This class will hold all the attributes and methods referent to a scv of an spacecraft
 */

#include "scv.h"

scv::scv(double px, double py, double pz, double vx, double vy, double vz, bool with_da)
{
    // Call to main allocator
    this->allocate_scv_values(px, py, pz, vx, vy, vz, with_da);
}

scv::scv(const DACE::AlgebraicVector<DACE::DA>& csv_DA)
{
    // Call to main allocator
    this->allocate_csv_DA_vector(csv_DA);
}

void scv::allocate_csv_DA_vector(const DACE::AlgebraicVector<DACE::DA>& csv_DA)
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
                    "Actual size: '%i'. Positions > 5 will be lost.", (int)csv_DA.size());
    }

    // Allocate variables
    this->px_ = std::make_shared<DACE::DA>(csv_DA[0]);
    this->py_ = std::make_shared<DACE::DA>(csv_DA[1]);
    this->pz_ = std::make_shared<DACE::DA>(csv_DA[2]);
    this->vx_ = std::make_shared<DACE::DA>(csv_DA[3]);
    this->vy_ = std::make_shared<DACE::DA>(csv_DA[4]);
    this->vz_ = std::make_shared<DACE::DA>(csv_DA[5]);
}

void scv::allocate_scv_values(double px, double py, double pz, double vx, double vy, double vz, bool with_da)
{
    // Allocate values
    this->px_ = std::make_shared<DACE::DA>(with_da ? px + DACE::DA(1) : px);
    this->py_ = std::make_shared<DACE::DA>(with_da ? py + DACE::DA(2) : py);
    this->pz_ = std::make_shared<DACE::DA>(with_da ? pz + DACE::DA(3) : pz);
    this->vx_ = std::make_shared<DACE::DA>(with_da ? vx + DACE::DA(4) : vx);
    this->vy_ = std::make_shared<DACE::DA>(with_da ? vy + DACE::DA(5) : vy);
    this->vz_ = std::make_shared<DACE::DA>(with_da ? vz + DACE::DA(6) : vz);
}

DACE::AlgebraicVector<DACE::DA> scv::get_state_vector()
{
    // Safety check, check that the pointers are not nullptr
    // TODO: create function here

    // Return scv vector
    DACE::AlgebraicVector<DACE::DA> res = {*this->px_,
                                           *this->py_,
                                           *this->pz_,
                                           *this->vx_,
                                           *this->vy_,
                                           *this->vz_};


    return res;
}

DACE::DA scv::get_parameter_copy(POSITION position)
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
    scv::check_mem_place(val_ptr.get());

    // Make a copy and exit
    return *val_ptr;
}

DACE::DA scv::get_parameter_copy(VELOCITY velocity)
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
    scv::check_mem_place(val_ptr.get());

    // Make a copy and exit
    return *val_ptr;

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

void scv::set_state_value(const DACE::DA& val, POSITION position)
{
    // Switch case
    switch (position)
    {
        case POSITION::X:
            this->px_ = std::make_shared<DACE::DA>(val);
            break;
        case POSITION::Y:
            this->py_ =  std::make_shared<DACE::DA>(val);
            break;
        case POSITION::Z:
            this->pz_ =  std::make_shared<DACE::DA>(val);
            break;
    }
}

void scv::set_state_value(const DACE::DA& val, VELOCITY velocity)
{
    // Switch case
    switch (velocity)
    {
        case VELOCITY::X:
            this->vx_ = std::make_shared<DACE::DA>(val);
            break;
        case VELOCITY::Y:
            this->vy_ =  std::make_shared<DACE::DA>(val);
            break;
        case VELOCITY::Z:
            this->vz_ =  std::make_shared<DACE::DA>(val);
            break;
    }
}

DACE::DA scv::get_state_value_copy(POSITION position)
{
    // Scope auxiliary variable
    DACE::DA res;

    // Switch case: make copy and assign result
    switch (position)
    {
        case POSITION::X:
            res = *this->px_;
            break;
        case POSITION::Y:
            res = *this->py_;
            break;
        case POSITION::Z:
            res = *this->pz_;
            break;
    }

    // Return result
    return res;
}

DACE::DA scv::get_state_value_copy(VELOCITY velocity) {
    // Scope auxiliary variable
    DACE::DA res;

    // Switch case: make copy and assign result
    switch (velocity)
    {
        case VELOCITY::X:
            res = *this->vx_;
            break;
        case VELOCITY::Y:
            res = *this->vy_;
            break;
        case VELOCITY::Z:
            res = *this->vz_;
            break;
    }

    // Return result
    return res;
}
