#ifndef HELPER_STRING_H
#define HELPER_STRING_H


#include <array>
#include <string>
#include <algorithm>

// ImGui heavily use C-style string for displaying and interacting with
// text. Passing raw pointers is tricky, so I prefer to use std::array with easy
// access to the underlying data with '.data()' and secure access with '.at()'.
// There is still the need to convert strings and arrays, however.


// Convert a 'std::string' to a 'std::array<char,N>'.
// If the string is smaller than N, the array will be padded with null
// characters.
// If the string is larger than N, the array data will be a null-terminated
// truncated version of the string.
template<size_t N>
constexpr std::array<char, N> string_to_array(std::string str)
{
    std::array<char, N> arr {};
    arr.fill('\0');
    str.resize(N-1, '\0');
    std::copy(str.begin(),
              std::find(str.begin(), str.end(), '\0'),
              arr.begin());
    return arr;
}

// Convert a 'std::array<char,N>' to a 'std::string'
// If the array has a null character, the string only contains the characters
// before it.
template<size_t N>
constexpr std::string array_to_string(const std::array<char, N>& arr)
{
    return {arr.begin(),
            std::find(arr.begin(), arr.end(), '\0')};
}


#endif // HELPER_STRING_H
