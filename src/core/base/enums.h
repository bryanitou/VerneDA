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
    RK78,
    NA
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
    OMPL_GAUSSIAN,
    NA,

};

/**
* Type of option inside delta generator
*/
enum class DELTA_GENERATOR_OPTION
{
    QUAT2EULER,
    ATTITUDE,
    TRANSLATION,
    QUATERNION_SAMPLING
};


/**
* Type of problem
*/
enum class PROBLEM
{
    TWO_BODY,
    FREE_TORQUE_MOTION,
    FREE_FALL_OBJECT,
    NA,
};

/**
* Units
*/
enum class LENGTH_UNITS
{
    METERS,
    KILOMETERS,
    NA
};


/**
* Sides
*/
enum class EVAL_TYPE
{
    DELTA,
    WALLS,
    CENTER
};

/**
 * Wall sides
 */
enum class WALL_SIDE
{
    UP,
    DOWN,
    RIGHT,
    LEFT
};

/**
* Algorithm to be used
*/
enum class ALGORITHM
{
    ADS,
    LOADS,
    NA
};

/**
* Left, right or center split
*/
enum class SPLITTING_PLACE
{
    LEFT,
    RIGHT,
    MIDDLE,
    NA
};