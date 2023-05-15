/**
 * ADS Test Propagation.
 */

// System libraries
#include <iostream>
#include <vector>
#include <cmath>
#include <fstream>

// DACE library
#include "dace/dace.h"

// ADS (Automatic Domain Splitting) libraries
#include "ads/Patch.h"
#include "ads/Manifold.h"

// Project libraries
#include "tools/vo.h"

template<typename T, typename U> DACE::AlgebraicVector<T> KeplerProp(DACE::AlgebraicVector<T> rv, U t, double mu)
{
    // Get the maximum order at which DACE have been initialized
    int ord = DACE::DA::getMaxOrder();

    // Set DACE final state vector
    DACE::AlgebraicVector<T> rv_fin(6);

    // Set initial position and initial velocity
    DACE::AlgebraicVector<T> rr0(3), vv0(3);

    // Get position
    rr0[0] = rv[0];
    rr0[1] = rv[1];
    rr0[2] = rv[2];

    // Get velocity
    vv0[0] = rv[3];
    vv0[1] = rv[4];
    vv0[2] = rv[5];

    /*
    for (int i=0; i<3; i++)
    {
        rr0[i] = rv[i];
        vv0[i] = rv[i+3];
    }
     */

    // Compute cross product between position and velocity: L = m (R x V) <- Angular momentum
    // TODO: Why the cross product is done?
    DACE::AlgebraicVector<T> hh = DACE::cross(rr0, vv0);

    // Convert results to constant strings
    auto rr_str = tools::vector::da_cons2string(rr0, ",", "%10.2f");
    auto vv_str = tools::vector::da_cons2string(vv0, ",", "%10.2f");
    auto hh_str = tools::vector::da_cons2string(hh, "," , "%10.2f");

    // Print resulting vector of the cross product
    std::fprintf(stdout, "Position (R)          : '%s' km\n", rr_str.c_str());
    std::fprintf(stdout, "Velocity (V)          : '%s' km s^-1\n", vv_str.c_str());
    std::fprintf(stdout, "R x V                 : '%s' km^2 s^-1 kg^-1\n", hh_str.c_str());

    // Get the norm of all the vectors
    auto h  = DACE::vnorm(hh);
    auto r0 = DACE::vnorm(rr0);
    auto v0 = DACE::vnorm(vv0);

    // Info
    std::fprintf(stdout, "Norm R                : '%f' km\n", r0.cons());
    std::fprintf(stdout, "Norm V                : '%f' km s^-1\n", v0.cons());
    std::fprintf(stdout, "Norm R x V            : '%f' km^2 s^-1 kg^-1\n", h.cons());

    // Compute some variables
    // Semi-major axis
    auto a = mu / (2 * mu / r0 - v0*v0);

    // Semi-latus rectum
    // See this web-page for further knowledge:
    // https://galileoandeinstein.phys.virginia.edu/7010/CM_14_Math_for_Orbits.html
    auto p = h*h / mu;

    // What is this?
    auto sigma0 = DACE::dot(rr0,vv0) / std::sqrt(mu);

    // Info
    std::fprintf(stdout, "Semi-major axis (a)   : '%f' km\n", a.cons());
    std::fprintf(stdout, "Semi-latum rectum (p) : '%f' km\n", p.cons());
    std::fprintf(stdout, "sigma0                : '%f' km kg^-2\n", sigma0.cons());

    // Tolerance
    double tol = 1.0;

    // Iterator
    int iter = 0;

    // SCL: it's not used, what is it?
    double scl = 1e-4;

    // Create F, Ft, G and Gt from template, which will be DACE::AlgebraicVector<T>
    T F, Ft, G, Gt;

    // Check if the semi-major access is positive
    if (DACE::cons(a)>0)
    {
        //
        T MmM0 = t * sqrt(mu/a/a/a);
        T EmE0 = cons(MmM0);
        //cout << "EmE0 " << cons(EmE0) << endl;

        // Iterative method to solve the implicit equation
        while ( tol > 1e-13 || iter < ord + 1)
        {
            //cout << iter << " " << ord << endl;
            iter++;
            T fx0 = -(MmM0) + (EmE0) + (sigma0)/sqrt((a))*(1 - cos((EmE0))) - (1-(r0)/(a)) * sin((EmE0));
            T fxp = 1 + (sigma0)/sqrt((a)) * sin((EmE0)) - (1-(r0)/(a)) * cos((EmE0));
            tol = abs(cons(fx0/fxp));
            EmE0 = EmE0 - fx0/fxp;
            //EmE0 = MmM0 - (sigma0)/sqrt((a))*(1 - cos((EmE0))) + (1-(r0)/(a)) * sin((EmE0));
        }
        //cout<<cons(EmE0)<<endl;
        /*
         //   cout << "EmE0 initial" << EmE0;
         for( iter=0; iter<ord; iter++){
         T fx0 = -(MmM0) + (EmE0) + (sigma0)/sqrt((a))*(1 - cos((EmE0))) - (1-(r0)/(a)) * sin((EmE0));
         T fxp = 1 + (sigma0)/sqrt((a)) * sin((EmE0)) - (1-(r0)/(a)) * cos((EmE0));
         EmE0 = EmE0 - fx0/fxp;
         cout << "fx0 "<< fx0 << endl;
         cout << "fxp "<< fxp << endl;
         }*/
        
        // cout << "EmE0 final" << EmE0;
        
        T theta = 2*atan2(sqrt(a*p)*tan(EmE0/2), r0 + sigma0*sqrt(a)*tan(EmE0/2));
        T r = p*r0 / (r0 + (p-r0)*cos(theta) - sqrt(p)*sigma0*sin(theta));
        
        //{compute the Lagrangian coefficients}
        F = 1 - a/r0 * (1 - cos(EmE0)) ;
        //cout << "F " << F << endl;
        G = a*sigma0/sqrt(mu)*(1 - cos(EmE0)) + r0 * sqrt(a/mu) * sin(EmE0);
        //cout << "G " << G.cons() << endl;
        Ft = - sqrt(mu*a)/(r*r0) * sin(EmE0);
        // cout << "Ft " << Ft.cons() << endl;
        Gt = 1 - a/r * (1-cos(EmE0));
        //cout << "Gt " << Gt.cons() << endl;
    }
    else
    {
        std::cout << "Negative semimajor axis" << std::endl;
        T NmN0 = t*sqrt(mu/(-a)/(-a)/(-a));
        T HmH0 = 0.0;
        
        //cout << "HmH0 " << HmH0;
        while(tol>1e-14 || iter < ord + 1)
        {
            //cout << iter << endl;
            iter ++;
            T fx0 = - (NmN0) - (HmH0) + (sigma0)/sqrt((-a)) * (-1 + cosh((HmH0))) + (1-(r0)/(a)) * sinh((HmH0)) ;
            T fxp = -1 + (sigma0)/sqrt((-a))*sinh((HmH0)) + (1-(r0)/(a))*cosh((HmH0));
            tol = abs(cons(fx0/fxp));
            HmH0 = HmH0 - fx0/fxp;
        }
        //cout << iter << endl;
        //cout << "HmH0 initial" << HmH0;
        
        for(iter=0; iter<ord; iter++)
        {
            //cout << iter << endl;
            T fx0 = - (NmN0) - HmH0 + (sigma0)/sqrt((-a)) * (-1 + cosh(HmH0)) + (1-(r0)/(a)) * sinh(HmH0) ;
            T fxp = -1 + (sigma0)/sqrt((-a))*sinh(HmH0) + (1-(r0)/(a))*cosh(HmH0);
            HmH0 = HmH0 - fx0/fxp;
            //cout << "fx0 "<< fx0 << endl;
            //cout << "fxp "<< fxp << endl;
            //cout << HmH0<< endl;
        }
        
        //cout << "HmH0 final" << HmH0;
        
        //cout << HmH0 << endl;
        //{DA expansion of HmH0 parameter}
        double Htemp, DE = 1.0;
        
        F = 1 - a/r0 * (1 - cosh(HmH0));
        //cout << "F " << F.cons() << endl;
        G = a*sigma0/sqrt(mu)*(1 - cosh(HmH0)) + r0 * sqrt(-a/mu) * sinh(HmH0);
        //cout << "G " << G.cons() << endl;
        DACE::AlgebraicVector<T> rv_temp(3);
        for (int i=0; i<3; i++) {
            rv_temp[i] = F * rr0[i] + G * vv0[i];
        }
        //cout << "rtemp" << rv_temp.cons() << endl;
        T r = vnorm(rv_temp);
        //cout << "r" << r.cons() << endl;
        Ft = - sqrt(mu*(-a))/(r*r0) * sinh(HmH0);
        //cout << "Ft" << Ft.cons() << endl;
        Gt = 1 - a/r*(1-cosh(HmH0));
        //cout << "Gt" << Gt.cons() << endl;
    }
    
    for (int i=0 ; i<3; i++)
    {
        rv_fin[i] = F * rr0[i] + G * vv0[i];
        //cout << cons(F * rr0[i]) << " " << cons(G * vv0[i]) <<endl;
        rv_fin[i+3] = Ft * rr0[i] + Gt * vv0[i];
        //cout << cons(Ft * rr0[i]) << " " << cons(Gt * vv0[i]) <<endl;
    }

    
    return rv_fin;
}


int main()
{
    // Set amount of order and variables
    int n_ord = 4;
    int n_var = 6;
    // Initialize order and amount of variables
    DACE::DA::init(n_ord, n_var);

    // Get amount of number of variables: TODO: seems to be useless
    int var = DACE::DA::getMaxVariables();

    // Which Epsilon is this one?
    // Docu: Set new truncation error and get the previous one
    double new_eps = 1e-40;
    double previous_eps = DACE::DA::setEps(new_eps);

    // Show to the used the new epsilon value
    std::fprintf(stdout, "Epsilon update: Previous: '%1.16f', New: '%1.16f'\n", previous_eps, new_eps);

    // Do we want to see the warnings? True (1) or False (0)
    DACE::DACEException::setWarning(true);

    // Set the precision of the doubles to printed
    std::cout.precision(10);

    // This block reads info from the txt file that contains observations-----------------------------------------------

    // Initialize AlgebraicVector
    DACE::AlgebraicVector<DACE::DA> rv(6);

    // Set state vector
    // INFO:    The initial state of the vector really depends on the statement of the problem, for this one:
    //
    //          Position (x, y, z) is a well known variable, hence no DA associated variable is added to it.
    //          Velocity (x, y, z) is not well known, then its stddev is associated to it!
    rv[0] = 39.9424547;         // Position x
    rv[1] = 42059.1319877;      // Position y
    rv[2] = 3065.4925715;       // Position z
    rv[3] = -3.0595482 + 0.01 * DACE::DA(1); // Speed x + variation
    rv[4] = -0.0189669 + 0.01 * DACE::DA(2); // Speed y + variation
    rv[5] =  0.2987034 + 0.01 * DACE::DA(3); // Speed z + variation

    // This is the mu in km^3 / s^
    double mu = 398600;

    // 3.3 days ?, 24 hours and 3600.0 seconds
    double DT = 3.3*24.0*3600.0;

    // Error tolerances and standard deviations Tolerances
    std::vector<double> errToll = {1e-3, 1e-3, 1e-3, 1e-6, 1e-6, 1e-6};

    // Num. split max -> Stands for exactly what?
    // INFO: This value is set as a maximum of splits per step
    int nSplitMax = 6 ;

    // Initialize patch
    // TODO: This is never used
    Patch set(rv);

    // Initialize manifold
    Manifold stack(rv);

    // Set clock
    // TODO: Unused
    clock_t split_time = clock();

    // Call to main running function
    // Manifold RVsplit = stack.getSplitDomain(KeplerProp, errToll, nSplitMax, DT,  mu );

    // Info to the user
    std::fprintf(stdout," It creates '%zu' domains.", RVsplit.size());

    // Close function with exit code 0
    return 0;
}



