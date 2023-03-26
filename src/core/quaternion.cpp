/**
 * Quaternions class.
 */

#include "quaternion.h"

std::vector<double> quaternion::euler2quaternion(double roll, double pitch, double yaw)
{
    // Pre-compute values for efficiency
    double sin_roll = sin(roll/2);
    double cos_roll = cos(roll/2);
    double sin_pitch = sin(pitch/2);
    double cos_pitch = cos(pitch/2);
    double sin_yaw = sin(yaw/2);
    double cos_yaw = cos(yaw/2);

    // Declare and initialize quaternion
    std::vector<double> q1(4);

    // Save the quaternion
    q1[1] = sin_roll * cos_pitch * cos_yaw - cos_roll * sin_pitch * sin_yaw;
    q1[2] = cos_roll * sin_pitch * cos_yaw + sin_roll * cos_pitch * sin_yaw;
    q1[3] = cos_roll * cos_pitch * sin_yaw - sin_roll * sin_pitch * cos_yaw;
    q1[0] = cos_roll * cos_pitch * cos_yaw + sin_roll * sin_pitch * sin_yaw;

    // Return got quaternion
    return q1;
}

std::vector<double> quaternion::euler2quaternion_fromGaussian(double x, double y, double z)
{
    // Compute theta
    double theta = std::sqrt(x * x + y * y + z * z);

    // Compute half theta
    double half_theta = theta / 2.0;

    // Compute s: TODO: What is s? I think it is an scaling value.
    double s = std::sin(half_theta) / theta;

    // Declare and initialize quaternion
    std::vector<double> q(4);

    // Pre-compute values for efficiency
    q[0] = cos(half_theta);
    q[1] = s * x;
    q[2] = s * y;
    q[3] = s * z;

    // Return got quaternion
    return q;
}

void quaternion::scale(DACE::AlgebraicVector<DACE::DA>* q, double num)
{
    // Save the quaternion
    q[0].normalize() = q[0] * num;
    q[1].normalize() = q[1] * num;
    q[2].normalize() = q[2] * num;
    q[3].normalize() = q[3] * num;

}

void quaternion::check_norm(DACE::AlgebraicVector<DACE::DA>* q)
{
    // Check the norm of the quaternion
    auto q_norm = q->cons().vnorm();
    auto line2write = tools::string::print2string("Integration point N^2(q) = '%.5f'", q_norm);

    // TODO: Debug line
    std::cout << line2write << std::endl;

    // Take square
    double qmagsq = q_norm*q_norm;

    // Check error
    double err = std::abs(1.0 - qmagsq);

    // Evaluate
    if (err < 2.107342e-08)
    {
        // Scale
        quaternion::scale(q, 2.0 / (1.0 + qmagsq));
    }
    else
    {
        // Scale
        quaternion::scale(q, 1.0 / std::sqrt(qmagsq));
    }
}

double quaternion::getnorm(std::vector<double> q)
{
    double norm =   q[0] * q[0]
                    + q[1] * q[1]
                    + q[2] * q[2]
                    + q[3] * q[3];

    return norm;
}


std::vector<double> quaternion::quaternion2euler(double w, double x, double y, double z)
{
    // Declare quaternion
    std::vector<double> euler_angles(3);

    // Auxiliary variables
    double num{};
    double den{};

    // Roll
    num = +2.0 * (w * x + y * z);
    den = +1.0 - 2.0 * (x * x + y * y);
    double roll = atan2(num, den);

    // Pitch
    num = +2.0 * (w * y - z * x);
    // Python code translated below: // TODO: Re-check this translation
    // double t2 = +1.0 if t2 > +1.0 else t2;
    // double t2 = -1.0 if t2 < -1.0 else t2;
    // Becomes:
    num = num > +1.0 ? +1.0 : num < -1.0 ? -1.0 : num;
    double pitch = asin(num);

    // Yaw
    num = +2.0 * (w * z + x * y);
    den = +1.0 - 2.0 * (y * y + z * z);
    double yaw = atan2(num, den);


    // Return Euler angles
    return std::vector<double> {roll, pitch, yaw};
}


std::vector<double>  quaternion::inverse(std::vector<double> q2inv)
{
    // New double inversed
    std::vector<double> q_inv(4);

    double norm = quaternion::getnorm(q2inv);

    q_inv[0] = q2inv[0] / norm;
    q_inv[1] = -q2inv[1] / norm;
    q_inv[2] = -q2inv[2] / norm;
    q_inv[3] = -q2inv[3] / norm;

    return q_inv;
}

std::vector<double> quaternion::q8_multiply(std::vector<double>  q1, std::vector<double>  q2)
{
    std::vector<double> q3(4);

    q3[0] = q1[0] * q2[0] - q1[1] * q2[1] - q1[2] * q2[2] - q1[3] * q2[3];
    q3[1] = q1[0] * q2[1] + q1[1] * q2[0] + q1[2] * q2[3] - q1[3] * q2[2];
    q3[2] = q1[0] * q2[2] - q1[1] * q2[3] + q1[2] * q2[0] + q1[3] * q2[1];
    q3[3] = q1[0] * q2[3] + q1[1] * q2[2] - q1[2] * q2[1] + q1[3] * q2[0];

    return q3;
}

std::vector<double> quaternion::q8_normal_01 (int seed )

//****************************************************************************80
//
//  Purpose:
//
//    Q8_NORMAL_01 returns a normally distributed quaternion.
//
//  Discussion:
//
//    The normal distribution with mean 0 and variance 1 is used.
//
//  Licensing:
//
//    This code is distributed under the GNU LGPL license.
//
//  Modified:
//
//    05 August 2018
//
//  Author:
//
//    John Burkardt
//
//  Parameters:
//
//    Input/output, int &SEED, a seed for the random number
//    generator.
//
//    Output, double *Q8_NORMAL_01[4], the sampled quaternion.
//
{
    auto r = r8vec_uniform_01_new ( 4, seed );

    std::vector<double> q(4);

    q[0] = std::sqrt ( - 2.0 * log ( r[0] ) ) * std::cos ( 2.0 * M_PI * r[1] );
    q[1] = std::sqrt ( - 2.0 * log ( r[0] ) ) * std::sin ( 2.0 * M_PI * r[1] );
    q[2] = std::sqrt ( - 2.0 * log ( r[2] ) ) * std::cos ( 2.0 * M_PI * r[3] );
    q[3] = std::sqrt ( - 2.0 * log ( r[2] ) ) * std::sin ( 2.0 * M_PI * r[3] );

    delete [] r;

    return q;
}


double *quaternion::r8vec_uniform_01_new ( int n, int &seed )

//****************************************************************************80
//
//  Purpose:
//
//    R8VEC_UNIFORM_01_NEW returns a new unit pseudorandom R8VEC.
//
//  Discussion:
//
//    This routine implements the recursion
//
//      seed = ( 16807 * seed ) mod ( 2^31 - 1 )
//      u = seed / ( 2^31 - 1 )
//
//    The integer arithmetic never requires more than 32 bits,
//    including a sign bit.
//
//  Licensing:
//
//    This code is distributed under the GNU LGPL license.
//
//  Modified:
//
//    19 August 2004
//
//  Author:
//
//    John Burkardt
//
//  Reference:
//
//    Paul Bratley, Bennett Fox, Linus Schrage,
//    A Guide to Simulation,
//    Second Edition,
//    Springer, 1987,
//    ISBN: 0387964673,
//    LC: QA76.9.C65.B73.
//
//    Bennett Fox,
//    Algorithm 647:
//    Implementation and Relative Efficiency of Quasirandom
//    Sequence Generators,
//    ACM Transactions on Mathematical Software,
//    Volume 12, Number 4, December 1986, pages 362-376.
//
//    Pierre L'Ecuyer,
//    Random Number Generation,
//    in Handbook of Simulation,
//    edited by Jerry Banks,
//    Wiley, 1998,
//    ISBN: 0471134031,
//    LC: T57.62.H37.
//
//    Peter Lewis, Allen Goodman, James Miller,
//    A Pseudo-Random Number Generator for the System/360,
//    IBM Systems Journal,
//    Volume 8, Number 2, 1969, pages 136-143.
//
//  Parameters:
//
//    Input, int N, the number of entries in the vector.
//
//    Input/output, int &SEED, a seed for the random number generator.
//
//    Output, double R8VEC_UNIFORM_01_NEW[N], the vector of pseudorandom values.
//
{
    int i;
    const int i4_huge = 2147483647;
    int k;
    double *r;

    if ( seed == 0 )
    {
        std::cerr << "\n";
        std::cerr << "R8VEC_UNIFORM_01_NEW - Fatal error!\n";
        std::cerr << "  Input value of SEED = 0.\n";
        exit ( 1 );
    }

    r = new double[n];

    for ( i = 0; i < n; i++ )
    {
        k = seed / 127773;

        seed = 16807 * ( seed - k * 127773 ) - k * 2836;

        if ( seed < 0 )
        {
            seed = seed + i4_huge;
        }

        r[i] = ( double ) ( seed ) * 4.656612875E-10;
    }

    return r;
}