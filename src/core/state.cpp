/**
 * This class will hold all the attributes and methods referent to a state of an spacecraft
 */

#include "state.h"

void state::allocate_state_values(double px, double py, double pz, double vx, double vy, double vz)
{
    // Allocate values
    this->px_ = std::make_shared<double>(px);
    this->py_ = std::make_shared<double>(py);
    this->pz_ = std::make_shared<double>(pz);
    this->vx_ = std::make_shared<double>(vx);
    this->vy_ = std::make_shared<double>(vy);
    this->vz_ = std::make_shared<double>(vz);
}

double state::get_parameter_copy(POSITION position)
{
    // Go to memory place
    std::shared_ptr<double> val_ptr;

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

    // Safety check
    if(val_ptr != nullptr)
    {
        // End program here
    }

    // Make a copy and exit
    return *val_ptr;
}

double state::get_parameter_copy(VELOCITY velocity)
{

}