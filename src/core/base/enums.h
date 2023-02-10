/**
* Library that will contain useful enums to be used alongside the whole project. For robustness purposes.
*/

# pragma once

/**
 * Position enum
 */
enum class POSITION
{
    X,
    Y,
    Z
};

/**
* Speed enum
*/
enum class VELOCITY
{
    X,
    Y,
    Z
};

/**
* Type of integrator
*/
enum class INTEGRATOR
{
    EULER,
    RK4
};