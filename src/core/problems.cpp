/**
 * NAMESPACE hosting problems formulas
 * - Quaternions problem: https://ieeexplore.ieee.org/stamp/stamp.jsp?tp=&arnumber=686806
 */

#include "problems.h"

problems::problems()
{
    // Initialize matrices
    int n = 3; // rows
    int m = 3; // cols

    // Initialize rows
    this->inertia_ = new double *[n];
    this->inverse_ = new double *[n];

    // For loop
    for (int i = 0; i < n; i++)
    {
        // For each row, allocate m columns
        this->inertia_[i] = new double [m];
        this->inverse_[i] = new double [m];
    }
}

problems::~problems() {
    // Initialize matrices
    int n = 3; // rows

    // For loop
    for (int i = 0; i < n; i++)
    {
        // For each row, allocate m columns
        delete[] this->inertia_[i];
        delete[] this->inverse_[i];
    }

    // Also delete main pointer
    delete[] this->inertia_;
    delete[] this->inverse_;
}


DACE::AlgebraicVector<DACE::DA> problems::TwoBodyProblem(DACE::AlgebraicVector<DACE::DA> scv, double t )
{
    // Create position and resultant vector
    DACE::AlgebraicVector<DACE::DA> pos(3), res(6);

    // Set positions: equal to the first three positions of the SCV (State Control Vector)
    pos[0] = scv[0]; // Px_dot
    pos[1] = scv[1]; // Py_dot
    pos[2] = scv[2]; // Pz_dot
    auto r = pos.vnorm();

    res[0] = scv[3]; // Px_dot = Vx
    res[1] = scv[4]; // Py_dot = Vy
    res[2] = scv[5]; // Pz_dot = Vz

    // Compute next Vx, Vy, Vz state from the current position
    res[3] = -constants::earth::mu*pos[0]/(r*r*r); // Vx_dot
    res[4] = -constants::earth::mu*pos[1]/(r*r*r); // Vy_dot
    res[5] = -constants::earth::mu*pos[2]/(r*r*r); // Vz_dot

    // Return result
    return res;
}

DACE::AlgebraicVector<DACE::DA> problems::FreeTorqueMotion(DACE::AlgebraicVector<DACE::DA> scv, double t )
{
    // Create attitude and resultant vector
    DACE::AlgebraicVector<DACE::DA> q(4), omega(3), res(7);

    // Set positions: equal to the first three positions of the SCV (State Control Vector)
    q[0] = scv[0]; // Previous state
    q[1] = scv[1]; // Previous state
    q[2] = scv[2]; // Previous state
    q[3] = scv[3]; // Previous state

    // Normalize vector
    // TODO: Do this will work? It seems so..! Otherwise think of using algorithm in function quaternion::check_norm
    q = q.normalize();

    omega[0] = scv[4];
    omega[1] = scv[5];
    omega[2] = scv[6];

    // Get the cross product result
    auto c = this->get_cross_product(omega);

    // Set result
    res[0] = 0.5 * ( omega[2] * q[1] - omega[1] * q[2] + omega[0] * q[3]); // theta_x_dot
    res[1] = 0.5 * (-omega[2] * q[0] + omega[0] * q[2] + omega[1] * q[3]); // theta_x_dot
    res[2] = 0.5 * ( omega[1] * q[0] - omega[0] * q[1] + omega[2] * q[3]); // theta_x_dot
    res[3] = 0.5 * (-omega[0] * q[0] - omega[1] * q[1] - omega[2] * q[2]); // theta_x_dot
    res[4] = this->inverse_[0][0] * c[0] + this->inverse_[0][1] * c[1] + this->inverse_[0][2] * c[2];; // omega_y_dot
    res[5] = this->inverse_[1][0] * c[0] + this->inverse_[1][1] * c[1] + this->inverse_[1][2] * c[2];; // omega_z_dot
    res[6] = this->inverse_[2][0] * c[0] + this->inverse_[2][1] * c[1] + this->inverse_[2][2] * c[2];; // omega_z_dot

    // Return result
    return res;
}

DACE::AlgebraicVector<DACE::DA> problems::get_cross_product(DACE::AlgebraicVector<DACE::DA> omega)
{
    DACE::AlgebraicVector<DACE::DA> b(3), c(3);

    // Compute I*omega
    b[0] = this->inertia_[0][0] * omega[0] + this->inertia_[0][1] * omega[1] + this->inertia_[0][2] * omega[2];
    b[1] = this->inertia_[1][0] * omega[0] + this->inertia_[1][1] * omega[1] + this->inertia_[1][2] * omega[2];
    b[2] = this->inertia_[2][0] * omega[0] + this->inertia_[2][1] * omega[1] + this->inertia_[2][2] * omega[2];

    // Compute cross product
    c[0] =   omega[1]*b[2] - b[1]*omega[2];
    c[1] = -(omega[0]*b[2] - b[2]*omega[2]);
    c[2] =   omega[0]*b[1] - b[0]*omega[1];

    // Return cross product
    return c;
}

void problems::set_inertia_matrix(double inertia[3][3])
{
    // Show info to the user
    for (int i = 0; i < 3; i++)
    {
        for (int j = 0; j < 3; j++)
        {
            this->inertia_[i][j] = inertia[i][j];
            std::fprintf(stdout, "DEBUG: Inertia matrix I['%d']['%d'] = '%.5f'", i, j, this->inertia_[i][j]);
        }
    }

    // Now, compute the inverse and set it
    // First of all, delete previous
    problems::memory_frees(this->inverse_);
    this->inverse_ = problems::get_inverse_matrix(this->inertia_);

    // Once done, print result as info
    for (int i = 0; i < 3; i++)
    {
        for (int j = 0; j < 3; j++)
        {
            std::fprintf(stdout, "DEBUG: Inertia inverse matrix I['%d']['%d'] = '%.5f'", i, j, this->inverse_[i][j]);
        }
    }
}

double** problems::get_inverse_matrix(double** a)
{
    // Get the determinant
    double det = problems::get_determinant(a);

    // Get the adjoin
    double a11 = a[1][1]*a[2][2] - a[2][1]*a[1][2];
    double a12 = a[0][2]*a[2][1] - a[2][2]*a[0][1];
    double a13 = a[0][1]*a[1][2] - a[1][1]*a[0][2];

    double a21 = a[1][2]*a[2][0] - a[2][2]*a[1][0];
    double a22 = a[0][0]*a[2][2] - a[2][0]*a[0][2];
    double a23 = a[0][2]*a[1][0] - a[1][2]*a[0][0];

    double a31 = a[1][0]*a[2][1] - a[2][0]*a[1][1];
    double a32 = a[0][1]*a[2][0] - a[2][1]*a[0][0];
    double a33 = a[0][0]*a[1][1] - a[1][0]*a[0][1];

    // Build adjoin matrix
    double adj_aux[3][3] = {
            {a11/det, a12/det, a13/det},
            {a21/det, a22/det, a23/det},
            {a31/det, a32/det, a33/det}
    };

    // Declare result
    auto** adj = new double * [3];

    // Initialize each column
    for (int i = 0; i < 3; i++)
    {
        adj[i] = new double [3];
        for (int j = 0; j < 3; j++)
        {
            adj[i][j] = adj_aux[i][j];
        }
    }

    // Now, should return result
    return adj;
}

double problems::get_determinant(double **a)
{
    double det = a[0][0] * (a[1][1]*a[2][2] - a[1][2]*a[2][1])
                 - a[0][1] * (a[1][0]*a[2][2] - a[2][0]*a[1][2])
                 + a[0][2] * (a[1][0]*a[2][1] - a[1][1]*a[2][0]);

    // Return determinant
    return det;
}

void problems::memory_frees(double **a)
{
    // Initialize matrices
    int n = 3; // rows

    // For loop
    for (int i = 0; i < n; i++)
    {
        // For each row, allocate m columns
        delete[] a[i];
    }

    // Also delete main pointer
    delete[] a;
}