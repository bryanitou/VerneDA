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
        const double S = 1367; // W/m^2
    }

    namespace moon
    {
        const double mu = 4.9028E12; // m^3 / s^2
        const double a[7] = {0,6.29,-1.27,0.66,0.21,-0.19,-0.11};
        const double b[7] = {218.32,135.0,259.3,235.7,269.9,357.5,106.5};
        const double c[7] = {481267.881,477198.87,-413335.36,890534.22,954397.74,35999.05,966404.03};
        const double d[7] = {0, 5.13,0.28,-0.28,-0.17,0,0};
        const double e[7] = {0,93.3,220.2,318.3,217.6,0,0};
        const double f[7] = {0,483202.02,960400.89,6003.15,-407332.21,0,0};
        const double g[7] = {0.9508,0.0518,0.0095,0.0078,0.0028,0,0};
        const double h[7] = {0,135.0,259.3,253.7,269.9,0,0};
        const double k[7] = {0,477198.87,-413335.38,890534.22,954397.7,0,0};


    }
}