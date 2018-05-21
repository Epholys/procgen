#include "helper_string.h"

std::string& ltrim(std::string& str)
{
    auto it = std::find_if(str.begin(), str.end(), [](char c){ return !std::isspace<char>(c, std::locale());});
    str.erase(str.begin(), it);
    return str;   
}

std::string& rtrim(std::string& str)
{
    auto it = std::find_if(str.rbegin(), str.rend(), [](char c){ return !std::isspace<char>(c, std::locale());});
    str.erase(it.base(), str.end());
    return str;   
}

std::string& trim(std::string& str)
{
    return ltrim(rtrim(str));
}

