/**
 * VO: Vector Operations space. Namespace dedicated to tools. -> Templates place
 */

template<typename T>
std::string tools::vector::num2string(std::vector<T> v, const std::string& separator, const std::string& precision,
                                      bool close)
{
    // Auxiliary local variables
    std::string result{};

    // Open vector at the beginning
    result += close ? "[" : "";

    // Do precision?
    bool do_precision = !precision.empty();

    // Safety check
    if (v.empty())
    {
        result += "]";
        return result;
    }

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
    result += close ? "]" : "";

    // Return string
    return result;
}

template<typename T>
std::string tools::vector::unwrapMxN(int M, int N, T** matrix,
                                     const std::string& separator, const std::string& precision,
                                     bool close)
{
    // Auxiliary local variables
    std::string result{};

    // Open vector at the beginning
    result += close ? "[" : "";

    // Do precision?
    bool do_precision = !precision.empty();

    // Auxiliary local var
    T pos;

    int m;
    int n;
    for (m = 0; m < M; m++)
    {
        for (n = 0; n < N; n++)
        {
            pos = matrix[m][n];
            result += do_precision ? tools::string::print2string(precision, pos) : std::to_string(pos);
            result += m == M-1 and n == N-1 ? "" : n == N-1 ? ";" : separator;
        }

        // Jump to next row
        result += m == M-1 ? close ? "]" : "" : "\n";
    }

    return result;
}