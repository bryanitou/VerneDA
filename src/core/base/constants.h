/**
 * Constants header.
 */
#pragma once

namespace constants
{
    namespace gravity
    {
        // Gravitational constant
        const double G = 6.6743E-11; // m^3 kg^-1 s^-2

        // Velocity of the light
        const double c = 299792.458; // km/s
    }
    namespace earth
    {
        // Earth radius
        const double radius = 6371E3; // m

        // Earth constants
        const double mass = 5.972E24; // kg

        // mu: G * mass
        // const double mu = 398600.4418 * 1000 * 1000 * 1000; //gravity::G * earth::mass; // m^3 / s^2
        const double mu = gravity::G * earth::mass; //gravity::G * earth::mass; // m^3 / s^2

        // local gravity at surface
        const double g = constants::earth::mu / (constants::earth::radius * constants::earth::radius);

        // J2 coefficient
        const double J2 = 1.08263E-3;
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