/**
 * Class having several types of integrators
 */

#include "integrator.h"

integrator::integrator(INTEGRATOR integrator, double stepmax)
{
    // Set type
    this->type = integrator;

    // Set step max
    this->hmax = stepmax;
}


DACE::AlgebraicVector<DACE::DA> integrator::euler(DACE::AlgebraicVector<DACE::DA> x,
                                                  double t0, double t1) const
{
    // Num of steps
    int steps = std::ceil((t1-t0)/this->hmax);

    // Delta time? TODO: Investigate what was this
    double h = (t1 - t0) / steps;

    // Current time
    double t = t0;

    // Iterate
    for( int i = 0; i < steps; i++ )
    {
        x = x + h * (probl_->solve(x, t));
        t += h;
    }

    return x;
}

DACE::AlgebraicVector<DACE::DA> integrator::RK4(DACE::AlgebraicVector<DACE::DA> x,
                                                  double t0, double t1) {
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

    // Num of steps
    int steps = std::ceil((t1-t0)/this->hmax);

    // Delta time? TODO: Investigate what was this
    double h = (t1 - t0) / steps;

    // Current time
    double t = t0;

    // Iterate
    for( int i = 0; i < steps; i++ )
    {
        // Print detailed info
        this->print_detailed_information(x, i, t);

        // Compute points in between
        k1 = probl_->solve(x, t);
        k2 = probl_->solve(x + h * (k1/3), t + h/3);
        k3 = probl_->solve(x + h * (-k1/3 + k2), t + 2*h/3);
        k4 = probl_->solve(x + h * (k1 - k2 + k3), t + h);

        // Compute the single step
        x = x + h * (k1 + 3*k2 + 3*k3 + k4)/8;

        // Increase step time
        t += h;
    }


    return x;
}

void integrator::print_detailed_information(const DACE::AlgebraicVector<DACE::DA>& x, int i, double t)
{
    // Get the information of x
    auto str2debug = tools::vector::da_cons2string(x, ", ", "%3.8f");

    // Common info shared on bot attitude and orbit determination
    auto str2print = tools::string::print2string("TRACE: i: %6d | t: %10.2f | v: %s", i, t,
                                                 str2debug.c_str());

    // If it is attitude, go this way...
    if (this->probl_->get_type() == PROBLEM::FREE_TORQUE_MOTION)
    {
        // Extract the quaternion from here if attitude
        auto q_cons = x.cons().extract(0, 3);

        // Extract the Euler angles if attitude
        auto euler2debug = quaternion::quaternion2euler(q_cons[0], q_cons[1], q_cons[2], q_cons[3]);

        // Euler to string
        auto str4euler = tools::vector::num2string<double>(euler2debug, ", ");

        str2print += tools::string::print2string(" | euler: %s", str4euler.c_str());
    }

    // Debug information
    std::fprintf(stdout,"%s\n", str2print.c_str());

}

DACE::AlgebraicVector<DACE::DA> integrator::integrate(const DACE::AlgebraicVector<DACE::DA>& x, double t0,
                                                      double t1)

{
    DACE::AlgebraicVector<DACE::DA> result;

    switch (this->type)
    {
        case INTEGRATOR::EULER:
        {
            result = this->euler(x,  t0, t1);
            break;
        }
        case INTEGRATOR::RK4:
        {
            result = this->RK4(x, t0, t1);
            break;
        }
        case INTEGRATOR::RK78:
        {
            result = this->RK78(6, x, t0, t1);
            break;
        }
        default:
        {
            // TODO: Add any fallback here.
            break;
        }

    }

    return result;
}

void integrator::set_problem_object(problems *probl)
{
    // Set problem pointer
    this->probl_ = probl;

    // Get the type of problem inserted
    auto problem_type = this->probl_->get_type();

    // Set the amount of variables needed
    this->nvar_ = problem_type == PROBLEM::FREE_TORQUE_MOTION ? 7 :
                  problem_type == PROBLEM::TWO_BODY ? 6 : 0;

    // Safety check
    if (this->nvar_ == 0)
    {
        std::fprintf(stderr, "Couldn't set the amount of variables to be used in the integrator. N. of "
                             "variables: '%d'", this->nvar_);
    }
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

template<typename T> DACE::AlgebraicVector<T> integrator::RK78(int N, DACE::AlgebraicVector<T> Y0, double X0, double X1)
{
    // TODO: Investigate what is this
    double ERREST;
    double H0 = 0.001;
    double HS = 0.1;
    double H1 = 100.0;
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

    H = std::abs(HS);
    HH0 = std::abs(H0);
    HH1 = std::abs(H1);
    X = X0;
    RFNORM = 0.0;
    ERREST = 0.0;

    // Iteration
    int idx = 0;

    while(X != X1)
    {
        // Print detailed info
        this->print_detailed_information(Y1, idx, X);

        // compute new stepsize
        if (RFNORM != 0)
        {
            // TODO: What is the difference between std::min and min ?
            H = H*min(4.0,exp(HSQR*log(EPS/RFNORM)));
        }
        if (std::abs(H) > std::abs(HH1))
        {
            H = HH1;
        }
        else if (std::abs(H) < std::abs(HH0)*0.99 )
        {
            H = HH0;
            std::cout << "--- WARNING, MINIMUM STEPSIZE REACHED IN RK" << std::endl;
        }

        if ((X+H-X1)*H>0)
        {
            H = X1-X;
        }

        for (J = 0; J<13; J++)
        {
            for (I = 0; I<N; I++)
            {
                Y0[I] = 0.0 ; // EVALUATE RHS AT 13 POINTS

                for (K=0; K<J; K++)
                {
                    Y0[I] = Y0[I] + Z[I][K+3]*B[J][K];
                }
                Y0[I] = H*Y0[I] + Z[I][0];
            }

            Y1 = probl_->solve(Y0, X+H*A[J]);

            for (I = 0; I<N; I++)
            {
                Z[I][J+3] = Y1[I];
            }
        }

        for (I = 0; I<N; I++)
        {
            Z[I][1] = 0.0 ; Z[I][2] = 0.0 ; // EXECUTE 7TH,8TH ORDER STEPS

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
            Y1cons[I] = cons(Y1[I]);
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

        idx++;
    }

    for (I = 0; I<N; I++)
    {
        Y1[I] = Z[I][0];
    }

    return Y1;

}
