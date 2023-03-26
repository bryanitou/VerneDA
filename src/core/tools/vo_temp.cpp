/**
 * VO: Vector Operations space. Namespace dedicated to tools. -> Templates place
 */

template<typename T>
std::string tools::vector::num2string(std::vector<T> v, const std::string& separator)
{
    // Auxiliary local variables
    std::string result{};

    // Open vector at the beginning
    result = "[";

    // Iterate through every position
    for (int i = 0; i < v.size() - 1; i++)
    {
        // Write value
        result += std::to_string(v[i]);
        result += separator;
    }

    // Close vector at the back
    result += std::to_string(v.back()) + "]";

    // Return string
    return result;
}