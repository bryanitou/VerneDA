/**
 * Quaternions class.
 */

#include "quaternion.h"

#include <utility>

template<typename T> std::vector<T> quaternion::euler2quaternion_temp(T roll, T pitch, T yaw)
{
    // Pre-compute values for efficiency
    auto sr = sin(roll * 0.5);
    auto cr = cos(roll * 0.5);
    auto sp = sin(pitch * 0.5);
    auto cp = cos(pitch * 0.5);
    auto sy = sin(yaw * 0.5);
    auto cy = cos(yaw * 0.5);

    // Declare and initialize quaternion
    std::vector<T> q1(4);

    // Save the quaternion
    q1[0] = cr * cp * cy + sr * sp * sy;
    q1[1] = sr * cp * cy - cr * sp * sy;
    q1[2] = cr * sp * cy + sr * cp * sy;
    q1[3] = cr * cp * sy - sr * sp * cy;

    // Return got quaternion
    return q1;
}

std::vector<double> quaternion::euler2quaternion(double roll, double pitch, double yaw)
{
    return quaternion::euler2quaternion_temp(roll, pitch, yaw);
}

std::vector<DACE::DA> quaternion::euler2quaternion(const DACE::DA& roll, const DACE::DA& pitch,const DACE::DA& yaw)
{
    return quaternion::euler2quaternion_temp(roll, pitch, yaw);
}


std::vector<double> quaternion::euler2quaternion_fromGaussian(double x, double y, double z)
{
    // Compute theta
    double theta = std::sqrt(x * x + y * y + z * z);

    // Compute half theta
    double half_theta = theta / 2.0;

    // Compute "s"
    double s = std::sin(half_theta) / theta;

    // Declare and initialize quaternion
    std::vector<double> q(4);

    // Pre-compute values for efficiency
    q[0] = std::cos(half_theta);
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

/*
std::vector<double> quaternion::quaternion2euler(double w, double x, double y, double z)
{
    // Auxiliary variable
    double num, den, token;
    double roll, pitch, yaw;

    // Roll
    num = +2.0 * (w * x + y * z);
    den = +1.0 - 2.0 * (x * x + y * y);
    roll = atan2(num, den);

    // Pitch
    num = std::sqrt(1 + 2 * (w * y - x * z));
    token = (1 - 2 * (w * y - x * z));
    den = std::sqrt(token > 0.0 ? token : 0.0);
    pitch = 2 * std::atan2(num, den) - M_PI / 2;

    // Yaw
    num = +2.0 * (w * z + x * y);
    den = +1.0 - 2.0 * (y * y + z * z);
    yaw = std::atan2(num, den);


    // Return Euler angles
    return std::vector<double> {roll, pitch, yaw};
}*/

std::vector<double> quaternion::quaternion2euler(std::vector<double> nq)
{
    return quaternion2euler_NORMAL(nq[0], nq[1], nq[2], nq[3]);
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