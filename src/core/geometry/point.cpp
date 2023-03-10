/**
 * Base point class.
 */

#include "point.h"

point::point(double x, double y, double z, const std::string& id)
{
    // Set the point and the id
    this->set_point(x, y, z);
    this->id = id;
}

void point::set_point(double x, double y, double z)
{
    this->p[0] = x;
    this->p[1] = y;
    this->p[2] = z;
}

void point::clear_point(double x, double y, double z)
{
    std::printf("Clearing point '%s' is being updated from", this->id.c_str());

    this->p[0] = 0.0;
    this->p[1] = 0.0;
    this->p[2] = 0.0;
}

std::vector<double> point::get_coordinates()
{
    return this->p;
}

void point::update_point(double x, double y, double z)
{
    // Info
    std::printf("Updating point '%s' is being updated from ['%.2f', '%.2f', '%.2f'] to ['%.2f', '%.2f', '%.2f']",
                this->id.c_str(),
                this->operator>(POSITION::X),
                this->operator>(POSITION::Y),
                this->operator>(POSITION::Z),
                x, y, z);

    // Set new point
    this->set_point(x, y, z);
}

double point::operator>(POSITION position)
{
    // Safety check its a possible value
    if (position == POSITION::NA)
    {
        std::printf("Couldn't get position!");
        std::exit(-1);
    }

    // Return result
    return position == POSITION::X ? this->p[0] :
           position == POSITION::Y ? this->p[1] : this->p[2];
}

bool point::operator==(point* point1)
{
    return ( this->operator>(POSITION::X) == point1->operator>(POSITION::X) ) &&
           ( this->operator>(POSITION::Y) == point1->operator>(POSITION::Y) ) &&
           ( this->operator>(POSITION::Z) == point1->operator>(POSITION::Z) );
}


