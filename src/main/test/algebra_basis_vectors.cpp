#include "dace/dace.h"
#include "tools/math.h"
#include "tools/vo.h"
#include "delta.h"

static void get_matrix_test(unsigned int n_var, DACE::DA res[])
{
    for (int i = 0; i < n_var; i++)
    {
        for (int j = 0; j < n_var; j++)
        {
            res[i*n_var + j]= i*j;
        }
    }
}

int main()
{
    DACE::DA::init(2, 6);

    // Order of the expansion
    auto n_ord = (int) DACE::DA::getMaxOrder();
    auto n_var = (int) DACE::DA::getMaxVariables();

    DACE::AlgebraicVector<DACE::DA> x0(6), xf(6), hh(3);

    // Set initial conditions
    const double mu = 398600;
    const double ecc = 0.5;

    // Initial conditions
    x0[0] = 6678.0 +    DACE::DA(1); // 300 km altitude
    x0[1] = 0.0    +    DACE::DA(2);
    x0[2] = 0.0    +    DACE::DA(3);
    x0[3] = 0.0    +    DACE::DA(4);
    x0[4] = sqrt(mu/6678.0)*sqrt(1+ecc) + DACE::DA(5);
    x0[5] = 0.0 + DACE::DA(6);

    // Get the basis of this algebra
    auto basis = tools::math::get_DA_basis(n_var, n_ord);
    auto kronecker = tools::math::Kroneckerproduct_vect(x0, x0);
    DACE::DA dydy3[n_var * n_var];
    get_matrix_test(n_var, dydy3);

    // Print them
    for (int i = 0; i < basis.size(); i++)
    {
        std::fprintf(stdout, "%3d: %s\n", i, tools::vector::num2string(basis[i]).c_str());
    }

}


