/**
 * Class having several types of integrators
 */

#include "integrator.h"
#include "ads/Patch.h"

integrator::integrator(INTEGRATOR integrator, ALGORITHM algorithm, double stepmax)
{
    // Set type
    this->type = integrator;

    // Set step max
    this->hmax_ = stepmax;

    // Set the algorithm
    this->algorithm_ = algorithm;
}


DACE::AlgebraicVector<DACE::DA> integrator::Euler(DACE::AlgebraicVector<DACE::DA> x)
{
    // Auxiliary previous state
    auto x_prev = x;

    // Auxiliary bool
    bool flag_interruption_errToll;

    // Iterate
    for(int i = 0; i < this->steps_; i++ )
    {
        // Print detailed info
        this->print_detailed_information(x, i, this->t_);

        // Get new polynomial
        x = this->Euler_step(x, this->t_, this->h_);

        // Check ADS conditions to continue integration
        if (this->interrupt_)
        {
            // Check returned flag
            flag_interruption_errToll = this->check_conditions(x);

            // Break integration if needed
            if (flag_interruption_errToll)
            {
                // Set result to the previous one
                x = x_prev;
                break;
            }
        }

        // Increase step time
        this->t_ += this->h_;

        // Update previous
        x_prev = x;
    }

    return x;
}

DACE::AlgebraicVector<DACE::DA> integrator::Euler_step(const DACE::AlgebraicVector<DACE::DA>& x, double t, double h) const
{
    return x + h * (this->problem_->solve(x, t));
}

DACE::AlgebraicVector<DACE::DA> integrator::RK4(DACE::AlgebraicVector<DACE::DA> x)
{
    // Set not end
    this->end_ = false;

    // Auxiliary previous state
    auto x_prev = x;

    // Auxiliary bool
    bool flag_interruption_errToll;

    // Auxiliary variable for debug
    std::string str2debug;
    std::string str4euler;
    std::vector<double> euler2debug;
    DACE::AlgebraicVector<double> q_cons;

    // Auxiliary variables for the loop
    DACE::AlgebraicVector<DACE::DA> k1;
    DACE::AlgebraicVector<DACE::DA> k2;
    DACE::AlgebraicVector<DACE::DA> k3;
    DACE::AlgebraicVector<DACE::DA> k4;

    // Iterate
    for(int i = 0; this->t_ < this->t1_; i++)
    {
        // Print detailed info
        this->print_detailed_information(x_prev, i, this->t_);

        // Compute the single step
        x = this->RK4_step(x_prev, this->t_, this->h_);

        // Check ADS conditions to continue integration
        if (this->interrupt_)
        {
            // Check returned flag
            flag_interruption_errToll = this->check_conditions(x, true);

            // Break integration if needed
            if (flag_interruption_errToll && this->interrupt_)
            {
                // Set result to the previous one
                x = x_prev;
                break;
            }
        }

        // Increase step time
        this->t_ += this->h_;

        // Update previous for next iteration
        x_prev = x;
    }

    // Check end condition
    this->end_ = this->t_ > this->t1_;

    // Return state
    return x;
}

DACE::AlgebraicVector<DACE::DA> integrator::RK4_step(const DACE::AlgebraicVector<DACE::DA>& x, double t, double h)
{
    // Compute points in between
    auto k1 = this->problem_->solve(x, t);
    auto k2 = this->problem_->solve(x + h * (k1/3), t + h/3);
    auto k3 = this->problem_->solve(x + h * (-k1/3 + k2), t + 2*h/3);
    auto k4 = this->problem_->solve(x + h * (k1 - k2 + k3), t + h);

    // Compute the single step
    return x + h * (k1 + 3*k2 + 3*k3 + k4)/8;
}

void integrator::print_detailed_information(const DACE::AlgebraicVector<DACE::DA>& x, int i, double t)
{
    // Get the information of x
    auto str2debug = tools::vector::da_cons2string(x, ", ", "%3.8f");

    // Patch or not?
    auto str2print = this->patch_id_ > -1 ? tools::string::print2string("p: %6d | ", this->patch_id_) : "";

    // Common info shared on bot attitude and orbit determination
    str2print += tools::string::print2string("i: %6d | t: %10.6f | v: %s", i, t, str2debug.c_str());

    // Add 'TRACE' in front
    str2print = "TRACE: " + str2print;

    // std::ofstream outfile;
//
    // outfile.open("out/example/attitude_test_RK4/att_integration/test_"+std::to_string(this->patch_id_)+".csv", std::ios_base::app); // append instead of overwrite

    // If it is attitude, go this way...
    if (this->problem_->get_type() == PROBLEM::FREE_TORQUE_MOTION)
    {
        // Extract the quaternion from here if attitude
        auto q_cons = x.cons().extract(0, 3);

        // Measure the norm
        auto q_norm = q_cons.vnorm();

        // Extract the Euler angles if attitude
        auto euler2debug = quaternion::quaternion2euler(q_cons[0], q_cons[1], q_cons[2], q_cons[3]);

        // Euler to string
        auto str4euler = tools::vector::num2string<double>(euler2debug, ", ", "%2.2f", false);

        str2print += tools::string::print2string(" | q_norm: %.2f | Euler: [%s]", q_norm, str4euler.c_str());

        // outfile << str4euler << std::endl;

        // if (q_cons.vnorm() < 0.9)
        // {
        //     bool stop = 21;
        // }
    }

    // Debug information
    std::fprintf(stdout,"%s\n", str2print.c_str());

}

void integrator::set_integration_parameters(const DACE::AlgebraicVector<DACE::DA>& scv0, double t0, double t1,
                                            bool interrupt)
{
    // We only set this parameters once
    if (!this->params_set_)
    {
        // Setting initial scv
        this->scv_ = scv0;

        // Set some values previous to the integration
        this->steps_ = std::ceil((t1 - t0) / this->hmax_);

        // Delta time? TODO: Investigate what was this
        this->h_ = (t1 - t0) / this->steps_;

        // Set times: initial and final
        this->t0_ = t0;
        this->t_ = t0;
        this->t1_ = t1;

        // Set interruption boolean
        this->interrupt_ = interrupt;

        // Parameters already set
        this->params_set_ = true;
    }
}


DACE::AlgebraicVector<DACE::DA> integrator::integrate(const DACE::AlgebraicVector<DACE::DA>& x, int patch_id)
{
    // Auxiliary variables
    DACE::AlgebraicVector<DACE::DA> result;

    // Set patch ID variable
    this->patch_id_ = patch_id;

    // Switch case
    switch (this->type)
    {
        case INTEGRATOR::EULER:
        {
            result = this->Euler(x);
            break;
        }
        case INTEGRATOR::RK4:
        {
            result = this->RK4(x);
            break;
        }
        case INTEGRATOR::RK78:
        {
            result = this->RK78(6, x);
            break;
        }
        default:
        {
            // TODO: Add any fallback here.
            break;
        }
    }

    // Return result
    return result;
}

problems *integrator::get_problem_ptr()
{
    // Safety check it is not empty
    if (this->problem_ == nullptr)
    {
        // Info
        std::fprintf(stdout, "Problem class to be returned is nullptr. Errors may occur later...\n");
    }

    return this->problem_;
}

void integrator::set_problem_ptr(problems *problem)
{
    // Set problem pointer
    this->problem_ = problem;

    // Get the type of problem inserted
    auto problem_type = this->problem_->get_type();

    // Set the amount of variables needed
    this->nvar_ = problem_type == PROBLEM::FREE_TORQUE_MOTION ? 7 :
                  problem_type == PROBLEM::TWO_BODY ? 6 :
                  problem_type == PROBLEM::FREE_FALL_OBJECT ? 6 : 0;

    // Safety check
    if (this->nvar_ == 0)
    {
        std::fprintf(stderr, "Couldn't set the amount of variables to be used in the integrator. N. of "
                             "variables: '%d'", this->nvar_);
    }
}

bool integrator::check_conditions(const DACE::AlgebraicVector<DACE::DA>& scv, bool debug)
{
    // Check the proper conditions to be checked in case ADS or LOADS
    switch (this->algorithm_)
    {
        case ALGORITHM::ADS:
        {
            // Check ADS conditions
            return this->check_ads_conditions(scv);
        }
        case ALGORITHM::LOADS:
        {
            // Check LOADS conditions
            return this->check_loads_conditions(scv, debug);
        }
        default:
        {
            // Show errs...
            std::fprintf(stderr, "You have not chosen the conditions to be checked... Whether ADS or LOADS.");

            // Exit code
            std::exit(50);
        }
    }

    return false;
}

bool integrator::check_ads_conditions(const DACE::AlgebraicVector<DACE::DA>& scv)
{
    // Result of the comparison
    bool result{false};

    // Auxiliary variables
    std::vector<double> truncation_errors(scv.size());

    // Iterate through every polynomial of the SCV
    for (unsigned int i = 0; i < scv.size(); ++i)
    {
        // Safety check
        if (scv[i].size() == 0) { continue; }

        // Get error
        auto errors = scv[i].estimNorm(0, 0, DACE::DA::getMaxOrder() + 1);

       //std::cout << tools::vector::num2string(err) << std::endl;

        // Get approximation error
        auto trunc_err2check = errors.back();

        // Save truncation errors for future treatment
        truncation_errors[i] = trunc_err2check;

        // Safety check
        if (std::isnan(trunc_err2check))
        {
            continue;
            // auto errmsg = tools::string::print2string("%s", scv[i].toString().c_str());
            // errmsg += "Got some nans in the error esimNorm computation. ";
            // std::fprintf(stderr, "TRACE: %s\n", errmsg.c_str());
        }

        // Compare error
        if (trunc_err2check > this->errToll_[i])
        {
            result = true;
            break;
        }
    }

    // Compute the splitting direction right now
    if (result)
    {
        // Compute relative error from truncation errors
        std::vector<double> relativErr(truncation_errors.size());

        for ( unsigned int k = 0; k < relativErr.size(); ++k )
        {
            relativErr[k] = truncation_errors[k] - this->errToll_[k];
        }
        for ( unsigned int k = 0; k < relativErr.size(); ++k )
        {
            if (truncation_errors[k] > this->errToll_[k])
            {
                relativErr[k] = DACE::abs(relativErr[k]);
            }
            else
            {
                relativErr[k] = 0.0;
            }
        }

        // Find maximum relative truncation error
        auto max_error = std::max_element(relativErr.begin(), relativErr.end());

        // TODO: Are we interested in this?
        // If position override is different than zero, set max error to that position
        // if ( posOverride != 0)
        // {
        //     max_error = (relativErr.begin() + posOverride);
        // }

        if (*max_error == 0.0)
        {
            bool a = true;
        }

        // Function component of maximum error
        const unsigned int pos = std::distance(relativErr.begin(), max_error);

        // Get the splitting direction
        this->pos_ = (int) Patch::getSplittingDirection(pos, scv) - 1;
    }

    // Return the errors
    return result;
}

bool integrator::check_loads_conditions(const DACE::AlgebraicVector<DACE::DA>& scv, bool debug)
{
    // Auxiliary variables
    int n_rows = (int) scv.size();
    int n_cols = (int) DACE::DA::getMaxVariables();

    // Result of the comparison
    bool result{false};

    // Jacobian matrix
    DACE::AlgebraicMatrix<DACE::DA> jacobian(n_rows, n_cols);

    // Upper bounds sum
    double upper_bound_sum = 0;
    double constant_sum = 0;

    // Reference: https://en.wikipedia.org/wiki/Jacobian_matrix_and_determinant
    for (int i = 0; i < n_rows; i++)
    {
        for (int j = 0; j < n_cols; j++)
        {
            // Check for non-zero values in order to avoid nans
            if (this->beta_[j] == 0.0)
            {
                continue;
            }

            // Make derivative
            auto comp_ij = (1/this->beta_[j]) * scv[i].deriv(j + 1);
            jacobian.at(i, j) = comp_ij;

            // Take constant part
            auto comp_ij_cons = comp_ij.cons();

            // Get first term
            auto comp_ij_first = comp_ij - comp_ij_cons;

            // Bounds computation
            auto bound_ij_ub = comp_ij_first.bound().m_ub;

            // Sum
            upper_bound_sum += bound_ij_ub * bound_ij_ub;
            constant_sum += comp_ij_cons * comp_ij_cons;
        }
    }

    // Compute the NLI
    this->nli_current_ = std::sqrt( upper_bound_sum / constant_sum );

    // Clear sum
    upper_bound_sum = 0.0;

    if (debug)
    {
        std::ofstream outfile;
        auto time_str = std::string (__TIME__);
        outfile.open("out/example/loads/nli_" + time_str + ".csv", std::ios_base::app); // append instead of overwrite

        // String to write
        auto str2write = tools::string::print2string(" %.16f, %.16f, %d", this->t_, this->nli_current_, this->patch_id_);

        // Write
        outfile << str2write << std::endl;
    }

    if (this->nli_current_ > this->nli_threshold_ || this->patch_id_ == 0 && (this->nli_current_ > 0.004) && false)
    {
        if (this->patch_id_ == 3)
        {
            bool a = true;
        }
        // It means we have exceeded the threshold!
        result = true;

        // Compute the direction
        // DACE::AlgebraicMatrix<DACE::DA> jacobian(n_rows, n_cols);
        DACE::AlgebraicVector<DACE::DA> v_list(n_cols, 0.0);
        std::vector<double> mu_list(n_cols);

        // Algorithm to decide which direction is the one that contributes the most
        for (int k = 0; k < n_cols; k++)
        {
            // Get direction to be evaluated
            v_list[k] = DACE::DA(k+1);

            for (int i = 0; i < n_rows; i++)
            {
                for (int j = 0; j < n_cols; j++)
                {
                    // Function composition
                    auto comp_ij = jacobian.at(i, j).eval(v_list);

                    // Get first term
                    auto comp_ij_first = comp_ij - comp_ij.cons();

                    // Bounds computation
                    auto bound_ij_ub = comp_ij_first.bound().m_ub;

                    // Sum
                    upper_bound_sum += bound_ij_ub * bound_ij_ub;
                }
            }

            // Save result
            mu_list[k] = std::sqrt( upper_bound_sum / constant_sum );

            // Clean stuff
            upper_bound_sum = 0.0;
            std::fill(v_list.begin(), v_list.end(), 0.0);
        }

        // Get the maximum
       this-> pos_ = (int) std::distance(mu_list.begin(), std::max_element(mu_list.begin(), mu_list.end()));
    }

    // Return the errors
    return result;
}


void integrator::set_errToll(const std::vector<double> &errToll)
{
    // Info
    auto vect2str = tools::vector::num2string(errToll, ", ");
    std::fprintf(stdout, "Setting the Error Tolerances vector to...: '%s'\n", vect2str.c_str());

    // Setting it...
    this->errToll_ = errToll;
}

void integrator::set_nli_threshold(const double &nli_threshold)
{
    // Info
    std::fprintf(stdout, "Setting the NLI threshold to...: '%.3f'\n", nli_threshold);

    // Setting it...
    this->nli_threshold_ = nli_threshold;
}

// Some auxiliary functions for the RK78, TODO: Can we get rid of min/max?
double min( double a, double b)
{
    double res = a;
    if(a>b){res = b;}
    return res;
}

double max( double a, double b)
{
    double res = a;
    if(b>a){res = b;}
    return res;
}


double normtmp( int N, std::vector<double> X)
{
    int I;
    double res = 0.0;

    for (I=0; I<N; I++)
    {
        if (X[I]<0)
        {
            X[I] = -X[I];
        }
        res = max(res,X[I]);
    }

    return res;
}

template<typename T> DACE::AlgebraicVector<T> integrator::RK78(int N, DACE::AlgebraicVector<T> Y0)
{
    // Auxiliary variables
    bool flag_interruption_errToll;

    // TODO: Investigate what is this
    double ERREST;
    double H0 = 0.001; // Minimum step size
    double HS = 0.1;   // Initial step size
    double H1 = 100.0; // Maximum step size
    double EPS = 1.e-12;
    double BS = 20*EPS;

    // TODO: Investigate what is this
    //  This is not initialized!
    int I, J, K;

    // AlgebraicMatrix<T> Z(N,16);
    std::vector<std::vector<T>> Z;

    // Reserve memory for optimization purposes
    Z.reserve(N);

    // Emplace back Vector of 16 positions
    for( I = 0; I < N; I++ )
    {
        Z.emplace_back(std::vector<T> (16));
    }


    DACE::AlgebraicVector<T> Y1(N);
    std::vector<double> Y1cons(N);

    // TODO: Investigate what is this
    double VIHMAX = 0.0, X, H;
    double RFNORM, HH0, HH1;

    double HSQR = 1.0/9.0;

    // This is a bit dangerous,
    // TODO: Initialize variables? -> Zeros are placed afterwards... Seems that no position is forgot
    double A[13], C[13], D[13];
    double B[13][12];

    // TODO: This A, B and C matrix are constant, can we move them to a header just for this RK78?
    // Set the A matrix values
    A[0] = 0.0; A[1] = 1.0/18.0; A[2] = 1.0/12.0; A[3] = 1.0/8.0; A[4] = 5.0/16.0; A[5] = 3.0/8.0;
    A[6] = 59.0/400.0; A[7] = 93.0/200.0; A[8] = 5490023248.0/9719169821.0; A[9] = 13.0/20.0;
    A[10] = 1201146811.0/1299019798.0; A[11] = 1.0; A[12] = 1.0;

    // Set the B matrix values
    // B Matrix: 1st. row
    B[0][0] = 0.0; B[0][1] = 0.0; B[0][2] = 0.0; B[0][3] = 0.0; B[0][4] = 0.0;
    B[0][5] = 0.0; B[0][6] = 0.0; B[0][7] = 0.0; B[0][8] = 0.0; B[0][9] = 0.0;
    B[0][10] = 0.0; B[0][11] = 0.0;

    // B Matrix: 2nd. row
    B[1][0] = 1.0/18.0; B[1][1] = 0.0; B[1][2] = 0.0; B[1][3] = 0.0; B[1][4] = 0.0;
    B[1][5] = 0.0; B[1][6] = 0.0; B[1][7] = 0.0; B[1][8] = 0.0; B[1][9] = 0.0;
    B[1][10] = 0.0; B[1][11] = 0.0;

    // B Matrix: 3rd. row
    B[2][0] = 1.0/48.0; B[2][1] = 1.0/16.0; B[2][2] = 0.0; B[2][3] = 0.0; B[2][4] = 0.0;
    B[2][5] = 0.0; B[2][6] = 0.0; B[2][7] = 0.0; B[2][8] = 0.0; B[2][9] = 0.0;
    B[2][10] = 0.0; B[2][11] = 0.0;

    // B Matrix: 4th. row
    B[3][0] = 1.0/32.0; B[3][1] = 0.0; B[3][2] = 3.0/32.0; B[3][3] = 0.0; B[3][4] = 0.0;
    B[3][5] = 0.0; B[3][6] = 0.0; B[3][7] = 0.0; B[3][8] = 0.0; B[3][9] = 0.0;
    B[3][10] = 0.0; B[3][11] = 0.0;

    // B Matrix: 5th. row
    B[4][0] = 5.0/16.0; B[4][1] = 0.0; B[4][2] = -75.0/64.0; B[4][3] = 75.0/64.0; B[4][4] = 0.0;
    B[4][5] = 0.0; B[4][6] = 0.0; B[4][7] = 0.0; B[4][8] = 0.0; B[4][9] = 0.0;
    B[4][10] = 0.0; B[4][11] = 0.0;

    // B Matrix: 6th. row
    B[5][0] = 3.0/80.0; B[5][1] = 0.0; B[5][2] = 0.0; B[5][3] = 3.0/16.0; B[5][4] = 3.0/20.0;
    B[5][5] = 0.0; B[5][6] = 0.0; B[5][7] = 0.0; B[5][8] = 0.0; B[5][9] = 0.0;
    B[5][10] = 0.0; B[5][11] = 0.0;

    // B Matrix: 7th. row
    B[6][0] = 29443841.0/614563906.0; B[6][1] = 0.0; B[6][2] = 0.0; B[6][3] = 77736538.0/692538347.0; B[6][4] = -28693883.0/1125000000.0;
    B[6][5] = 23124283.0/1800000000.0; B[6][6] = 0.0; B[6][7] = 0.0; B[6][8] = 0.0; B[6][9] = 0.0;
    B[6][10] = 0.0; B[6][11] = 0.0;

    // B Matrix: 8th. row
    B[7][0] = 16016141.0/946692911.0; B[7][1] = 0.0; B[7][2] = 0.0; B[7][3] = 61564180.0/158732637.0; B[7][4] = 22789713.0/633445777.0;
    B[7][5] = 545815736.0/2771057229.0; B[7][6] = -180193667.0/1043307555.0; B[7][7] = 0.0; B[7][8] = 0.0; B[7][9] = 0.0;
    B[7][10] = 0.0; B[7][11] = 0.0;

    // B Matrix: 9th. row
    B[8][0] = 39632708.0/573591083.0; B[8][1] = 0.0; B[8][2] = 0.0; B[8][3] = -433636366.0/683701615.0; B[8][4] = -421739975.0/2616292301.0;
    B[8][5] = 100302831.0/723423059.0; B[8][6] = 790204164.0/839813087.0; B[8][7] = 800635310.0/3783071287.0; B[8][8] = 0.0; B[8][9] = 0.0;
    B[8][10] = 0.0; B[8][11] = 0.0;

    // B Matrix: 10th. row
    B[9][0] = 246121993.0/1340847787.0; B[9][1] = 0.0; B[9][2] = 0.0; B[9][3] = -37695042795.0/15268766246.0; B[9][4] = -309121744.0/1061227803.0;
    B[9][5] = -12992083.0/490766935.0; B[9][6] = 6005943493.0/2108947869.0; B[9][7] = 393006217.0/1396673457.0; B[9][8] = 123872331.0/1001029789.0; B[9][9] = 0.0;
    B[9][10] = 0.0; B[9][11] = 0.0;

    // B Matrix: 11th. row
    B[10][0] = -1028468189.0/846180014.0; B[10][1] = 0.0; B[10][2] = 0.0; B[10][3] = 8478235783.0/508512852.0; B[10][4] = 1311729495.0/1432422823.0;
    B[10][5] = -10304129995.0/1701304382.0; B[10][6] = -48777925059.0/3047939560.0; B[10][7] = 15336726248.0/1032824649.0; B[10][8] = -45442868181.0/3398467696.0; B[10][9] = 3065993473.0/597172653.0;
    B[10][10] = 0.0; B[10][11] = 0.0;

    // B Matrix: 12th. row
    B[11][0] = 185892177.0/718116043.0; B[11][1] = 0.0; B[11][2] = 0.0; B[11][3] = -3185094517.0/667107341.0; B[11][4] = -477755414.0/1098053517.0;
    B[11][5] = -703635378.0/230739211.0; B[11][6] = 5731566787.0/1027545527.0; B[11][7] = 5232866602.0/850066563.0; B[11][8] = -4093664535.0/808688257.0; B[11][9] = 3962137247.0/1805957418.0;
    B[11][10] = 65686358.0/487910083.0; B[11][11] = 0.0;

    // B Matrix: 13th. row
    B[12][0] = 403863854.0/491063109.0; B[12][1] = 0.0; B[12][2] = 0.0; B[12][3] = - 5068492393.0/434740067.0; B[12][4] = -411421997.0/543043805.0;
    B[12][5] = 652783627.0/914296604.0; B[12][6] = 11173962825.0/925320556.0; B[12][7] = -13158990841.0/6184727034.0; B[12][8] = 3936647629.0/1978049680.0; B[12][9] = -160528059.0/685178525.0;
    B[12][10] = 248638103.0/1413531060.0; B[12][11] = 0.0;

    // Set the C matrix values
    C[0] = 14005451.0/335480064.0; C[1] = 0.0; C[2] = 0.0; C[3] = 0.0; C[4] = 0.0; C[5] = -59238493.0/1068277825.0;
    C[6] = 181606767.0/758867731.0; C[7] = 561292985.0/797845732.0; C[8] = -1041891430.0/1371343529.0; C[9] = 760417239.0/1151165299.0; C[10] = 118820643.0/751138087.0; C[11] = -528747749.0/2220607170.0;
    C[12] = 1.0/4.0;

    // Set the D matrix values
    D[0] = 13451932.0/455176623.0; D[1] = 0.0; D[2] = 0.0; D[3] = 0.0; D[4] = 0.0; D[5] = -808719846.0/976000145.0;
    D[6] = 1757004468.0/5645159321.0; D[7] = 656045339.0/265891186.0; D[8] = -3867574721.0/1518517206.0; D[9] = 465885868.0/322736535.0; D[10] = 53011238.0/667516719.0; D[11] = 2.0/45.0;
    D[12] = 0.0;

    for( I = 0; I < N; I++ )
    {
        Z[I][0] = Y0[I];
        Z[I][1] = 0.0 ;
    }

    H = std::abs(HS); // Initial step size
    HH0 = std::abs(H0); // Minimum step size
    HH1 = std::abs(H1); // Maximum step size
    X = this->t_;
    RFNORM = 0.0;
    ERREST = 0.0;

    // Iteration
    int idx = 0;

    // Auxiliary previous state
    auto x_prev = Y0;

    // Integrate until X reaches the desired time
    while(X != this->t1_)
    {
        // Print detailed info
        this->print_detailed_information(Y0, idx, X);

        // compute new stepsize
        if (RFNORM != 0)
        {
            // TODO: What is the difference between std::min and min ?
            H = H*min(4.0,exp(HSQR*log(EPS/RFNORM)));
        }

        // If the stepsize is larger in absolute value than HH1, limit it to HH1
        if (std::abs(H) > std::abs(HH1))
        {
            H = HH1;
        }
        else if (std::abs(H) < std::abs(HH0)*0.99 )
        {
            H = HH0;
            std::cout << "--- WARNING, MINIMUM STEPSIZE REACHED IN RK" << std::endl;
        }

        if ((X+H-this->t1_)*H>0)
        {
            H = this->t1_-X;
        }

        for (J = 0; J<13; J++)
        {
            for (I = 0; I<N; I++)
            {
                Y0[I] = 0.0; // EVALUATE RHS AT 13 POINTS

                for (K=0; K<J; K++)
                {
                    Y0[I] = Y0[I] + Z[I][K+3]*B[J][K];
                }
                Y0[I] = H*Y0[I] + Z[I][0];
            }

            // TODO: Fix this
            Y1 = this->problem_->solve(Y0, X+H*A[J]);

            // std::cout << Y1 << std::endl;

            for (I = 0; I<N; I++)
            {
                Z[I][J+3] = Y1[I];
            }
        }

        for (I = 0; I<N; I++)
        {
            Z[I][1] = 0.0 ;
            Z[I][2] = 0.0 ; // EXECUTE 7TH,8TH ORDER STEPS

            for (J = 0; J<13; J++)
            {
                Z[I][1] = Z[I][1] + Z[I][J+3]*D[J];
                Z[I][2] = Z[I][2] + Z[I][J+3]*C[J];
            }

            Y1[I] = (Z[I][2]-Z[I][1])*H;
            Z[I][2] = Z[I][2]*H+Z[I][0];
        }


        for (I = 0; I<N; I++)
        {
            Y1cons[I] = DACE::cons(Y1[I]);
        }

        // ESTIMATE ERROR AND DECIDE ABOUT BACKSTEP
        RFNORM = normtmp(N,Y1cons);

        if ((RFNORM>BS) && (abs(H/H0)>1.2))
        {
            H = H/3.0;
            RFNORM = 0;
        }
        else
        {
            for (I = 0; I<N; I++) {Z[I][0] = Z[I][2];}
            X = X + H;
            VIHMAX = max(VIHMAX,H);
            ERREST = ERREST + RFNORM;
        }

        // Check ADS conditions to continue integration
        // TODO: Confirm x_prev, Y1check logic assignation, not tested.
        if (this->interrupt_)
        {
            DACE::AlgebraicVector<DACE::DA> Y1check(N);
            for (I = 0; I<N; I++) { Y1check[I] = Z[I][0]; }

            // Check returned flag
            flag_interruption_errToll = this->check_conditions(Y1check);

            // Break integration if needed
            if (flag_interruption_errToll)
            {
                Y1 = x_prev;
                break;
            }

            // Update next state
            x_prev = Y1check;
        }

        // Update times to the class
        this->t_ = X;
        this->h_ = H;

        idx++;
    }

    if (!flag_interruption_errToll)
    {
        for (I = 0; I<N; I++)
        {
            Y1[I] = Z[I][0];
        }
    }

    return Y1;

}
