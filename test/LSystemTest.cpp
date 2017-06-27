#include <gtest/gtest.h>

#include "LSystem.h"

using namespace lsys;

TEST(LSystemTest, string_to_vec)
{
    std::string str = "Test";
    std::vector<char> vec = { 'T', 'e', 's', 't' };
    
    ASSERT_EQ(vec, string_to_vec(str));
}

TEST(LSystemTest, default_ctor)
{
    LSystem lsys;
    
    LSystem::production_rules empty_rules;
    std::vector<char> empty_vec;
    
    ASSERT_EQ(lsys.get_axiom(), empty_vec);
    ASSERT_EQ(lsys.get_rules(), empty_rules);
    ASSERT_EQ(lsys.get_result(), empty_vec);
}

TEST(LSystemTest, complete_ctor)
{
    LSystem lsys ( string_to_vec("F"), { { 'F', string_to_vec("F+F") } } );

    LSystem::production_rules expected_rules = { { 'F', string_to_vec("F+F") } };

    ASSERT_EQ(lsys.get_axiom(), string_to_vec("F"));
    ASSERT_EQ(lsys.get_rules(), expected_rules);
    ASSERT_EQ(lsys.get_result(), string_to_vec("F"));
}

TEST(LSystemTest, pretty_ctor)
{
    LSystem lsys ( "F", { { 'F', "F+F" } } );

    LSystem::production_rules expected_rules = { { 'F', string_to_vec("F+F") } };

    ASSERT_EQ(lsys.get_axiom(), string_to_vec("F"));
    ASSERT_EQ(lsys.get_rules(), expected_rules);
    ASSERT_EQ(lsys.get_result(), string_to_vec("F"));
}

TEST(LSystemTest, derivation)
{
    LSystem lsys ( "F", { { 'F', "F+G" }, { 'G', "G-F" } } );

    std::vector<char> iter_1 = string_to_vec("F+G");
    std::vector<char> iter_3 = string_to_vec("F+G+G-F+G-F-F+G");

    ASSERT_EQ(lsys.iter(), iter_1);
    ASSERT_EQ(lsys.iter(2), iter_3);
}
        
