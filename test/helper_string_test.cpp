// TODO trim
#include <gtest/gtest.h>

#include "helper_string.h"

TEST(helper_string_test, string_to_array)
{
    std::string short_str = "short";
    std::string long_str  = "A longer string than before to compare";
    std::array<char, 8> short_expected { 's', 'h', 'o', 'r', 't', '\0', '\0', '\0' };
    std::array<char, 8> long_expected  { 'A', ' ', 'l', 'o', 'n', 'g' , 'e' , '\0' };
    
    ASSERT_EQ(string_to_array<8>(short_str), short_expected);
    ASSERT_EQ(string_to_array<8>(long_str), long_expected);
}

TEST(helper_string_test, array_to_string)
{
    std::array<char, 8> short_arr { 's', 'h', 'o', 'r', 't', '\0', '\0', '\0' };
    std::array<char, 8> long_arr  { 'A', ' ', 'l', 'o', 'n', 'g' , 'e' , 'r'  };
    std::string short_expected = "short";
    std::string long_expected  = "A longer";

    ASSERT_EQ(array_to_string(short_arr), short_expected);
    ASSERT_EQ(array_to_string(long_arr), long_expected);
}
