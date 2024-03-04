/**
 * VO: Vector Operations space. Namespace dedicated to tools.
 */

#include "vo.h"

std::string tools::vector::da_cons2string(const DACE::AlgebraicVector<DACE::DA>& v, const std::string& separator,
                                          const std::string& precision, bool close)
{
    // Get the constants from the vector
    auto v_cte = static_cast<std::vector<double>>(v.cons());

    // Call to already built template
    return tools::vector::num2string<double>(v_cte, separator, precision, close);
}

std::string tools::vector::DA2string(const DACE::DA &da)
{
    // Initialize result
    std::string str2return{};

    // Represent the DA variable in a linear taylor polynomial form
    // Get the DA n_ord and n_var
    auto n_var = (int) DACE::DA::getMaxVariables();
    auto n_ord = (int) DACE::DA::getMaxOrder();

    // Iterate through them
    str2return += std::to_string(da.cons());

    // Iterate through the DA variable


    return str2return;
}