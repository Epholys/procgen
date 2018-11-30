#include <sstream>
#include <gtest/gtest.h>
#include "cereal/archives/json.hpp"
#include "LSystem.h"


TEST(LSystemTest, default_ctor)
{
    LSystem lsys;
    
    LSystem::production_rules empty_rules;
    std::string empty_str;
    std::unordered_map<int, std::string> empty_prod_cache;
    std::unordered_map<int, std::vector<int>> empty_rec_cache;
    
    ASSERT_EQ(lsys.get_axiom(), empty_str);
    ASSERT_EQ(lsys.get_rules(), empty_rules);
    ASSERT_EQ(lsys.get_production_cache(), empty_prod_cache);
    ASSERT_EQ(lsys.get_recursion_predecessor(), empty_str);
    ASSERT_EQ(lsys.get_recursion_cache(), empty_rec_cache);
}

TEST(LSystemTest, complete_ctor)
{
    LSystem lsys { "F", { { 'F', "F+F" } }, "F" };
    LSystem::production_rules expected_rules = { { 'F', "F+F" } };
    std::vector<int> expected_recursion_cache = { 0 };

    ASSERT_EQ(lsys.get_axiom(), "F");
    ASSERT_EQ(lsys.get_production_rules(), expected_rules);
    ASSERT_EQ(lsys.get_production_cache().at(0), "F");
    ASSERT_EQ(lsys.get_recursion_predecessors(), "F");
    ASSERT_EQ(lsys.get_recursion_cache(), expected_recursion_cache);
}

TEST(LSystemTest, get_axiom)
{
    LSystem lsys { "F", { { 'F', "F+F" } }, "F"  };

    ASSERT_EQ(lsys.get_axiom(), "F");
}

TEST(LSystemTest, set_axiom)
{
    LSystem lsys { "F", { { 'F', "F+F" } }, "F"  };

    lsys.set_axiom("FF");

    ASSERT_EQ(lsys.get_axiom(),       "FF");
    ASSERT_EQ(lsys.get_cache().at(0), "FF");
}

TEST(LSystemTest, add_rule)
{
    LSystem lsys { "F", { }, "F"  };
    LSystem::production_rules expected_rules = { { 'F', "F+F" } };
    std::unordered_map<int, std::string> base_cache { { 0, "F" } };
    
    lsys.add_rule('F', "F+F");

    ASSERT_EQ(lsys.get_rules(), expected_rules);
    ASSERT_EQ(lsys.get_cache(), base_cache);
}

TEST(LSystemTest, remove_rule)
{
    LSystem lsys { "F", { { 'F', "F+F" } }, "F"  };
    LSystem::production_rules empty_rules;
    std::unordered_map<int, std::string> base_cache { { 0, "F" } };

    lsys.remove_rule('F');

    ASSERT_EQ(lsys.get_rules(), empty_rules);
    ASSERT_EQ(lsys.get_cache(), base_cache);

    ASSERT_THROW(lsys.remove_rule('G'), gsl::fail_fast);
}

TEST(LSystemTest, clear_rules)
{
    LSystem lsys { "F", { { 'F', "F+F" }, { 'G', "GG" } }, "F"  };
    LSystem::production_rules empty_rules;
    std::unordered_map<int, std::string> base_cache { { 0, "F" } };

    lsys.clear_rules();

    ASSERT_EQ(lsys.get_rules(), empty_rules);
    ASSERT_EQ(lsys.get_cache(), base_cache);
}

TEST(LSystemTest, set_recursion_predecessors)
{
    LSystem lsys { "F", { { 'F', "F+F" }, { 'G', "GG" } }, "F" };
    std::string expected_predecessors = "";
    std::unordered_map<int, std::vector<int>> empty_cache_;

    lsys.set_recursion_predecessors();
    ASSERT_EQ(lsys.get_recursion_predecessors(), expected_predecessors);
    ASSERT_EQ(lsys.get_recursion_cache, empty_cache);
}

// Test some iterations.
TEST(LSystemTest, derivation)
{
    LSystem lsys { "F", { { 'F', "F+G" }, { 'G', "G-F" } }, "F" };

    std::string prod_iter_1 = "F+G";
//    std::string prod_iter_2 = "F+G + G-F";
    std::string prod_iter_3 = "F+G + G-F  +  G-F - F+G";
    
    std::vector<int> rec_iter_1 = {1, 1, 1};
//    std::vector<int> rec_iter_2 = {2,2,2, 1, 1,1,1};
    std::vector<int> rec_iter_3 = {3,3,3, 2, 2,2,2,  1,  1,1,1, 1, 2,2,2};

    ASSERT_EQ(lsys.produce(1), {prod_iter_1, rec_iter_1});
    ASSERT_EQ(lsys.produce(3), {prod_iter_3, rec_iter_3});
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

TEST(LSystemTest, serialization)
{
    LSystem olsys ("FG", { {'F', "F+G"}, {'G', "G-F" } });
    LSystem ilsys;

    std::stringstream ss;
    {
        cereal::JSONOutputArchive oarchive (ss);
        oarchive(olsys);
    }
    {
        cereal::JSONInputArchive iarchive (ss);
        iarchive(ilsys);
    }

    ASSERT_EQ(olsys.get_axiom(), ilsys.get_axiom());
    ASSERT_EQ(olsys.get_rules(), ilsys.get_rules());
}
