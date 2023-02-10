/**
 * Constants header.
 */

namespace constants
{
    namespace gravity
    {
        // Gravitational constant
        const double G = 6.6743E-11; // m^3 kg^-1 s^-2
    }
    namespace earth
    {
        // Earth constants
        const double mass = 5.972E24; // kg

        // mu: G * mass
        const double mu = gravity::G * earth::mass; // m^3 / s^2
    }

    namespace sun
    {
        // Stuff here
    }

    namespace mars
    {
        // Stuff here
    }
}