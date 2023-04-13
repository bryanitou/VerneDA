/**
 * VO: Vector Operations space. Namespace dedicated to tools. -> Templates place
 */

template<typename T>
std::string tools::vector::num2string(std::vector<T> v, const std::string& separator, const std::string& precision)
{
    // Auxiliary local variables
    std::string result{};

    // Open vector at the beginning
    result = "[";

    // Do precision?
    bool do_precision = !precision.empty();

    // Iterate through every position
    for (int i = 0; i < v.size() - 1; i++)
    {
        // Write value
        result += do_precision ? tools::string::print2string(precision, v[i]) : std::to_string(v[i]);
        result += separator;
    }

    // Write last value
    result += do_precision ? tools::string::print2string(precision, v.back()) : std::to_string(v.back());

    // Close vector
    result += "]";

    // Return string
    return result;
}