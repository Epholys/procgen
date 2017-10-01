#include <gtest/gtest.h>

#include "LSystem.h"

using namespace lsys;

// Test the string_to_vec helper function.
TEST(LSystemTest, string_to_vec)
{
    std::string str = "Test";
    std::vector<char> vec = { 'T', 'e', 's', 't' };
    
    ASSERT_EQ(vec, string_to_vec(str));
}

// Test the vec_to_string helper function.
TEST(LSystemTest, vec_to_string)
{
    std::string str = "Test";
    std::vector<char> vec = { 'T', 'e', 's', 't' };
    
    ASSERT_EQ(str, vec_to_string(vec));
}

// Test the default constructor.
TEST(LSystemTest, default_ctor)
{
    LSystem lsys;
    
    LSystem::production_rules empty_rules;
    std::vector<char> empty_vec;
    std::unordered_map<int, std::vector<char>> empty_cache;
    
    ASSERT_EQ(lsys.get_axiom(), empty_vec);
    ASSERT_EQ(lsys.get_rules(), empty_rules);
    ASSERT_EQ(lsys.get_cache(), empty_cache);
}

// Test the complete constructor.
TEST(LSystemTest, complete_ctor)
{
    LSystem lsys ( string_to_vec("F"), { { 'F', string_to_vec("F+F") } } );

    LSystem::production_rules expected_rules = { { 'F', string_to_vec("F+F") } };

    ASSERT_EQ(lsys.get_axiom(),       string_to_vec("F"));
    ASSERT_EQ(lsys.get_rules(),       expected_rules);
    ASSERT_EQ(lsys.get_cache().at(0), string_to_vec("F"));
}

// Test the easy-to-use constructor.
TEST(LSystemTest, pretty_ctor)
{
    LSystem lsys ( "F", { { 'F', "F+F" } } );

    LSystem::production_rules expected_rules = { { 'F', string_to_vec("F+F") } };

    ASSERT_EQ(lsys.get_axiom(),       string_to_vec("F"));
    ASSERT_EQ(lsys.get_rules(),       expected_rules);
    ASSERT_EQ(lsys.get_cache().at(0), string_to_vec("F"));
}

// Test some iterations
TEST(LSystemTest, derivation)
{
    LSystem lsys ( "F", { { 'F', "F+G" }, { 'G', "G-F" } } );

    std::vector<char> iter_1 = string_to_vec("F+G");
    std::vector<char> iter_3 = string_to_vec("F+G+G-F+G-F-F+G");

    ASSERT_EQ(lsys.produce(1), iter_1);
    ASSERT_EQ(lsys.produce(3), iter_3);
}
        
