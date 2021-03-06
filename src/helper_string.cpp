#include "helper_string.h"

#include <cctype>
#include <utility>

std::string ltrim(std::string str)
{
    auto it = std::find_if(str.begin(), str.end(), [](char c) {
        return !std::isspace<char>(c, std::locale());
    });
    str.erase(str.begin(), it);
    return str;
}

std::string rtrim(std::string str)
{
    auto it = std::find_if(str.rbegin(), str.rend(), [](char c) {
        return !std::isspace<char>(c, std::locale());
    });
    str.erase(it.base(), str.end());
    return str;
}

std::string trim(std::string str)
{
    return ltrim(rtrim(std::move(str)));
}

std::string to_camel_case(std::string str)
{
    std::transform(begin(str), end(str), begin(str), [](char c) -> char {
        if (std::isspace(c) != 0)
            return '_';

        return std::tolower(c);
    });
    return str;
}
