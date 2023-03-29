/**
 * VO: Vector Operations space. Namespace dedicated to tools.
 */

#include "vo.h"

std::string tools::vector::da_cons2string(const DACE::AlgebraicVector<DACE::DA>& v, const std::string& separator,
                                          const std::string& precision)
{
    // Get the constants from the vector
    auto v_cte = static_cast<std::vector<double>>(v.cons());

    // Call to already built template
    return tools::vector::num2string<double>(v_cte, separator, precision);
}