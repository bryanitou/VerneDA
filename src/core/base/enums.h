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
    Z,
    NA
};

/**
* Speed enum
*/
enum class VELOCITY
{
    X,
    Y,
    Z,
    NA
};

/**
 * SCV: State Control Vector enum
 */
enum class STATE
{
    PX,
    PY,
    PZ,
    VX,
    VY,
    VZ
};

/**
* Type of integrator
*/
enum class INTEGRATOR
{
    EULER,
    RK4,
    RK78
};

enum class DISTRIBUTION
{
    GAUSSIAN,
    UNIFORM
};

/**
* Type of sampling for quaternion
*/
enum class QUATERNION_SAMPLING
{
    EULER_GAUSSIAN,
    SEED_GAUSSIAN,
    NA,

};