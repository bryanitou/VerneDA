/**
 * STR: String space. Namespace dedicated to tools.
 */

#include "str.h"

std::string tools::string::clean_bars(const std::string& str2clean)
{
    // Copy new string
    std::string str2return = str2clean;

    // Chars we want to remove:
    std::string chars2remove = tools::string::keys::bars + tools::string::keys::comma;

    // Remove characters
    for(char char2remove : chars2remove)
    {
        // Position to erase
        auto it = std::remove(str2return.begin(), str2return.end(), char2remove);

        // Erase it from the group
        str2return.erase(it, str2return.end());
    }

    // Return the string
    return str2return;
}

std::string tools::string::print2string(const std::string &fmt_str, ...)
{
    va_list ap;
    char *fp = nullptr;
    va_start(ap, fmt_str);
    vasprintf(&fp, fmt_str.c_str(), ap);
    va_end(ap);
    std::unique_ptr<char[]> formatted(fp);
    std::string message = std::string(formatted.get());
    return message;
}