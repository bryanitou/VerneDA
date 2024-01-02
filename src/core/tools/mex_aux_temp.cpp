/**
 * Templates
 */

template<typename T>
std::string mex_aux::toUTF8(const std::basic_string<T, std::char_traits<T>, std::allocator<T>> &source) {
    std::string result;

    std::wstring_convert<std::codecvt_utf8_utf16<T>, T> convertor;
    result = convertor.to_bytes(source);

    return result;
}

template<typename T>
void mex_aux::fromUTF8(const std::string &source, std::basic_string<T, std::char_traits<T>, std::allocator<T>> &result) {
    std::wstring_convert<std::codecvt_utf8_utf16<T>, T> convertor;
    result = convertor.from_bytes(source);
}
