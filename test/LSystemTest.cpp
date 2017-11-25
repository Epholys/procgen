#include <gtest/gtest.h>

#include "LSystem.h"

using namespace lsys;


TEST(LSystemTest, default_ctor)
{
    LSystem lsys;
    
    LSystem::production_rules empty_rules;
    std::string empty_vec;
    std::unordered_map<int, std::string> empty_cache;
    
    ASSERT_EQ(lsys.get_axiom(), empty_vec);
    ASSERT_EQ(lsys.get_rules(), empty_rules);
    ASSERT_EQ(lsys.get_cache(), empty_cache);
}

TEST(LSystemTest, complete_ctor)
{
    LSystem lsys { "F", { { 'F', "F+F" } } };

    LSystem::production_rules expected_rules = { { 'F', "F+F" } };

    ASSERT_EQ(lsys.get_axiom(),       "F");
    ASSERT_EQ(lsys.get_rules(),       expected_rules);
    ASSERT_EQ(lsys.get_cache().at(0), "F");
}

TEST(LSystemTest, set_axiom)
{
    LSystem lsys { "F", { { 'F', "F+F" } } };

    lsys.set_axiom("FF");

    ASSERT_EQ(lsys.get_axiom(),       "FF");
    ASSERT_EQ(lsys.get_cache().at(0), "FF");
}

TEST(LSystemTest, add_rule)
{
    LSystem lsys { "F", { } };
    LSystem::production_rules expected_rules = { { 'F', "F+F" } };

    lsys.add_rule('F', "F+F");

    ASSERT_EQ(lsys.get_rules(), expected_rules);
}

TEST(LSystemTest, remove_rule)
{
    LSystem lsys { "F", { { 'F', "F+F" } } };
    LSystem::production_rules empty_rules;

    lsys.remove_rule('F');

    ASSERT_EQ(lsys.get_rules(), empty_rules);
}


// Test some iterations.
TEST(LSystemTest, derivation)
{
    LSystem lsys { "F", { { 'F', "F+G" }, { 'G', "G-F" } } };

    std::string iter_1 = "F+G";
    std::string iter_3 = "F+G+G-F+G-F-F+G";

    ASSERT_EQ(lsys.produce(1), iter_1);
    ASSERT_EQ(lsys.produce(3), iter_3);
}

// Test some iterations in a non-standard order.
TEST(LSystemTest, wild_derivation)
{
    LSystem lsys { "F", { { 'F', "F+" } } };

    std::string iter_1 = "F+";
    std::string iter_3 = "F+++";
    std::string iter_5 = "F+++++";

    ASSERT_EQ(lsys.produce(3), iter_3);
    ASSERT_EQ(lsys.produce(1), iter_1);
    ASSERT_EQ(lsys.produce(5), iter_5);
}

