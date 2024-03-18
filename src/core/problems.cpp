/**
 * NAMESPACE hosting problems formulas
 * - Quaternions problem: https://ieeexplore.ieee.org/stamp/stamp.jsp?tp=&arnumber=686806
 */

#include "problems.h"

problems::problems(PROBLEM type, double mu)
{
    // Set problem type
    this->type_ = type;

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

    // Set mu
    this->mu_ = mu;
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


DACE::AlgebraicVector<DACE::DA> problems::TwoBodyProblem(DACE::AlgebraicVector<DACE::DA> scv, double t)const{
    // Create position and resultant vector
    DACE::AlgebraicVector<DACE::DA> pos(3), vel(3), res(6);

    // Set positions: equal to the first three positions of the SCV (State Control Vector)
    pos[0] = scv[0]; // Px_dot
    pos[1] = scv[1]; // Py_dot
    pos[2] = scv[2]; // Pz_dot
    auto r = pos.vnorm();

    vel[0] = scv[3]; // Vx
    vel[1] = scv[4]; // Vy
    vel[2] = scv[5]; // Vz
    auto V = vel.vnorm();

    res[0] = vel[0]; // Px_dot = Vx
    res[1] = vel[1]; // Py_dot = Vy
    res[2] = vel[2]; // Pz_dot = Vz

    // Compute next Vx, Vy, Vz state from the current position
    res[3] = -this->mu_*pos[0]/(r*r*r); // Vx_dot
    res[4] = -this->mu_*pos[1]/(r*r*r); // Vy_dot
    res[5] = -this->mu_*pos[2]/(r*r*r); // Vz_dot

    // Read perturbations from input object
    auto input_obj = json_parser::parse_input_file("/home/nerea/CLionProjects/VerneDA/examples/translation_loads.json");
    std::vector<std::string> perturbations = input_obj.initial_conditions.perturbations;

    if (std::find(perturbations.begin(), perturbations.end(),"J2") != perturbations.end()) {
        // Display in the terminal a message in order to show that J2 has been selected
        std::cout << "J2 perturbation has been selected." << std::endl;

        // Include J2 perturbation formula
        auto factor = (3.0 / 2.0) * constants::earth::mu* constants::earth::J2 * (constants::earth::radius*constants::earth::radius) / (r * r * r * r * r);

        auto a_J2_x = pos[0] * (5.0 * pos[2] * pos[2] / (r * r) - 1);
        auto a_J2_y = pos[1] * (5.0 * pos[2] * pos[2] / (r * r) - 1);
        auto a_J2_z = pos[1] * (5.0 * pos[2] * pos[2] / (r * r) - 3);

        // Update Vx, Vy, Vz
        res[3] += factor * a_J2_x; // Vx_dot
        res[4] += factor * a_J2_y; // Vy_dot
        res[5] += factor * a_J2_z; // Vz_dot
    }

    else if (std::find(perturbations.begin(), perturbations.end(),"DRAG") != perturbations.end()) {
        // Display in the terminal a message in order to show that DRAG has been selected
        std::cout << "Atmospheric drag perturbation has been selected." << std::endl;

        double Cd = input_obj.initial_conditions.drag_coefficient;
        double A = input_obj.initial_conditions.cross_sectional_area;
        double m = input_obj.initial_conditions.mass;
        double rho = input_obj.initial_conditions.atmospheric_density;

        // Ensure non-zero values to avoid division by zero
        if (Cd == 0.0 || A == 0.0 || m == 0.0 || rho == 0.0) {
                std::cerr << "Error: Invalid parameters for atmospheric drag." << std::endl;
        }
        else {
            // Calculate atmospheric drag acceleration
            auto a_drag_x = -0.5*rho*V*Cd*A*vel[0]/m;
            auto a_drag_y = -0.5*rho*V*Cd*A*vel[1]/m;
            auto a_drag_z = -0.5*rho*V*Cd*A*vel[2]/m;

            // Add the acceleration due to atmospheric drag to the velocity components
            res[3] += a_drag_x; // Vx_dot
            res[4] += a_drag_y; // Vy_dot
            res[5] += a_drag_z; // Vz_dot
        }
    }

    else if (std::find(perturbations.begin(), perturbations.end(),"SOLAR_RADIATION") != perturbations.end()) {
        // Display in the terminal a message in order to show that SOLAR_RADIATION has been selected
        std::cout << "Solar radiation pressure perturbation has been selected." << std::endl;

        double reflectionFactor = input_obj.initial_conditions.reflection_factor;
        double A = input_obj.initial_conditions.cross_sectional_area;
        double m = input_obj.initial_conditions.mass;

        // Ensure non-zero values to avoid division by zero
        if (reflectionFactor == 0.0 || A == 0.0 || m == 0.0) {
            std::cerr << "Error: Invalid parameters for solar radiation pressure." << std::endl;
        }
        else {

            // Calculate solar radiation pressure acceleration
            auto a_solar_x = -reflectionFactor*A*A*pos[0]/(constants::gravity::c*m);
            auto a_solar_y = -reflectionFactor*A*A*pos[1]/(constants::gravity::c*m);
            auto a_solar_z = -reflectionFactor*A*A*pos[2]/(constants::gravity::c*m);

            // Add the acceleration due to solar radiation pressure to the velocity components
            res[3] += a_solar_x; // Vx_dot
            res[4] += a_solar_y; // Vy_dot
            res[5] += a_solar_z; // Vz_dot
            }
    }
    else {
        std::cout << "Warning: perturbation not found." << std::endl;
    }
    // Return result
    return res;
}

DACE::AlgebraicVector<DACE::DA> problems::pol2cart(DACE::AlgebraicVector<DACE::DA> pol){
    // Create position and resultant vector
    DACE::AlgebraicVector<DACE::DA> cart(2);

    // Make computations
    cart[0] = pol[0] * DACE::cos(pol[1]);
    cart[1] = pol[0] * DACE::sin(pol[1]);

    // Return result
    return cart;
}

DACE::AlgebraicVector<DACE::DA> problems::FreeFallObject(DACE::AlgebraicVector<DACE::DA> scv, double t){
    // Create position and resultant vector
    DACE::AlgebraicVector<DACE::DA> pos(3), vel(3), res(6);

    // Set positions: equal to the first three positions of the SCV (State Control Vector)
    pos[0] = scv[0]; // Px_dot
    pos[1] = scv[1]; // Py_dot
    pos[2] = scv[2]; // Pz_dot

    // Take speeds
    vel[0] = scv[3];
    vel[1] = scv[4];
    vel[2] = scv[5];
    auto vel_norm = vel.vnorm();

    res[0] = scv[3]; // Px_dot = Vx
    res[1] = scv[4]; // Py_dot = Vy
    res[2] = scv[5]; // Pz_dot = Vz

    // Compute some constants TODO: Remove this hard set stuff and let user decide
    double area = 10; // m^2
    double mass = 10; // kg
    double weight = mass * constants::earth::g;

    // Compute friction
    auto friction = vel_norm*vel_norm * area / mass - weight / mass;

    // Compute next Vx, Vy, Vz state from the current position
    res[3] = - area*vel[0]*vel[0] / mass * (vel[0] / DACE::abs(vel[0])); // Vx_dot
    res[4] = - area*vel[1]*vel[1] / mass * (vel[1] / DACE::abs(vel[1])); // Vy_dot
    res[5] = - area*vel[2]*vel[2] / mass * (vel[2] / DACE::abs(vel[2])) - constants::earth::g; // Vz_dot

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
    // TODO: Remove this, causes non linearity
    // q = q / q.vnorm().cons(); // This way doesn't brake linearity

    omega[0] = scv[4];
    omega[1] = scv[5];
    omega[2] = scv[6];

    // Get the cross product result
    auto c = this->get_cross_product(omega);

    // Set result
    // TODO: Fix this equation from politecnico di torino paper
    res[0] = 0.5 * (       0.0       - omega[0] * q[1] - omega[1] * q[2] - omega[2] * q[3]);
    res[1] = 0.5 * ( omega[0] * q[0] +       0.0       + omega[2] * q[2] - omega[1] * q[3]);
    res[2] = 0.5 * ( omega[1] * q[0] - omega[2] * q[1] +       0.0       + omega[0] * q[3]);
    res[3] = 0.5 * ( omega[2] * q[0] + omega[1] * q[1] - omega[2] * q[2] +       0.0      );
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
        // Info
        std::fprintf(stdout, "DEBUG: Inertia matrix I[%d][j]: ", i);

        for (int j = 0; j < 3; j++)
        {
            this->inertia_[i][j] = inertia[i][j];
            std::fprintf(stdout, " %10.2f", this->inertia_[i][j]);
        }

        // Go next row
        std::fprintf(stdout, "\n");
    }

    // Now, compute the inverse and set it
    // First of all, delete previous
    problems::memory_frees(this->inverse_);
    this->inverse_ = problems::get_inverse_matrix(this->inertia_);

    // Once done, print result as info
    for (int i = 0; i < 3; i++)
    {
        // Info
        std::fprintf(stdout, "DEBUG: Inverse inertia matrix I^-1[%d][j]: ", i);

        for (int j = 0; j < 3; j++)
        {
            std::fprintf(stdout, " %12.8f", this->inverse_[i][j]);
        }

        // Go next row
        std::fprintf(stdout, "\n");
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

DACE::AlgebraicVector<DACE::DA> problems::solve(const DACE::AlgebraicVector<DACE::DA>& scv, double t)
{
    // Result here
    DACE::AlgebraicVector<DACE::DA> res;

    switch (this->type_)
    {
        case PROBLEM::TWO_BODY:
        {
            // Call Free Torque Motion problem
            res = problems::TwoBodyProblem(scv, t);
            break;
        }
        case PROBLEM::FREE_TORQUE_MOTION:
        {
            // Call Free Torque Motion problem
            res = this->FreeTorqueMotion(scv, t);
            break;
        }
        case PROBLEM::FREE_FALL_OBJECT:
        {
            // Call to Free Fall Object problem
            res = this->FreeFallObject(scv, t);
            break;
        }
        case PROBLEM::POL2CART:
        {
            // Call to Polar to Cartesian transformation
            res = this->pol2cart(scv);
            break;
        }
        default:
        {
            // Info
            std::fprintf(stdout, "Should be Two Body Problem or Free Torque Motion Problem.\n");
            break;
        }
    }
    return res;
}


void problems::summary(std::string * summary2return, bool recursive)
{
    // Check if this module is summary to be launched
    if (!this) {
        *summary2return += tools::string::print2string("Problems (%p): is nullptr.\n", this);

        // Return
        return;
    }

    // Pointers
    *summary2return += tools::string::print2string("Problems (%p): inertia flag set to '%p'\n",
                                                   this, this->inertia_);

    *summary2return += tools::string::print2string("Problems (%p): inverse flag set to '%p'\n",
                                                   this, this->inverse_);

    // DOUBLE
    *summary2return += tools::string::print2string("Problems (%p): mu flag set to '%.2f'\n",
                                                   this, this->mu_);

    // ENUMS
    *summary2return += tools::string::print2string("Problems (%p): type flag set to '%s'\n",
                                                   this, tools::enums::PROBLEM2str(this->type_).c_str());

    // Recursive...
    if (recursive)
    {
        auto inertia_str = tools::vector::unwrapMxN(3, 3, this->inertia_);
        auto inverse_str = tools::vector::unwrapMxN(3, 3, this->inverse_);
        *summary2return += tools::string::print2string("Problems (%p): inertia matrix: \n%s\n", this, inertia_str.c_str());
        *summary2return += tools::string::print2string("Problems (%p): inverse matrix: \n%s\n", this, inverse_str.c_str());
    }

}