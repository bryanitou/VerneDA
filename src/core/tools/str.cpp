/**
 * STR: String space. Namespace dedicated to tools.
 */

#include "str.h"

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