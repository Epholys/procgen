#include <gtest/gtest.h>

#include "LSystem.h"

using namespace std::string_literals;

TEST(LSystemTest, default_ctor)
{
    LSystem lsys;
    std::unordered_map<char, std::string> empty_rules;
    ASSERT_EQ(lsys.get_axiom(), ""s);
    ASSERT_EQ(lsys.get_rules(), empty_rules);
    ASSERT_EQ(lsys.get_result(), ""s);
}
