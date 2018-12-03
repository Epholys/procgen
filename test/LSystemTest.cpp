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
    std::unordered_map<int, std::pair<std::vector<int>, int>> empty_rec_cache;
    
    ASSERT_EQ(lsys.get_axiom(), empty_str);
    ASSERT_EQ(lsys.get_rules(), empty_rules);
    ASSERT_EQ(lsys.get_production_cache(), empty_prod_cache);
    ASSERT_EQ(lsys.get_iteration_predecessors(), empty_str);
    ASSERT_EQ(lsys.get_iteration_cache(), empty_rec_cache);
}

TEST(LSystemTest, complete_ctor)
{
    LSystem lsys { "F", { { 'F', "F+F" } }, "F" };
    LSystem::production_rules expected_rules = { { 'F', "F+F" } };
    std::unordered_map<int, std::pair<std::vector<int>, int>> expected_recursion_cache =
        { {0, {{0}, 0}} };

    ASSERT_EQ(lsys.get_axiom(), "F");
    ASSERT_EQ(lsys.get_rules(), expected_rules);
    ASSERT_EQ(lsys.get_production_cache().at(0), "F");
    ASSERT_EQ(lsys.get_iteration_predecessors(), "F");
    ASSERT_EQ(lsys.get_iteration_cache(), expected_recursion_cache);
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
    ASSERT_EQ(lsys.get_production_cache().at(0), "FF");
}

TEST(LSystemTest, add_rule)
{
    LSystem lsys { "F", { }, "F"  };
    LSystem::production_rules expected_rules = { { 'F', "F+F" } };
    std::unordered_map<int, std::string> base_cache { { 0, "F" } };
    
    lsys.add_rule('F', "F+F");

    ASSERT_EQ(lsys.get_rules(), expected_rules);
    ASSERT_EQ(lsys.get_production_cache(), base_cache);
}

TEST(LSystemTest, remove_rule)
{
    LSystem lsys { "F", { { 'F', "F+F" } }, "F"  };
    LSystem::production_rules empty_rules;
    std::unordered_map<int, std::string> base_cache { { 0, "F" } };

    lsys.remove_rule('F');

    ASSERT_EQ(lsys.get_rules(), empty_rules);
    ASSERT_EQ(lsys.get_production_cache(), base_cache);

    ASSERT_THROW(lsys.remove_rule('G'), gsl::fail_fast);
}

TEST(LSystemTest, clear_rules)
{
    LSystem lsys { "F", { { 'F', "F+F" }, { 'G', "GG" } }, "F"  };
    LSystem::production_rules empty_rules;
    std::unordered_map<int, std::string> base_cache { { 0, "F" } };

    lsys.clear_rules();

    ASSERT_EQ(lsys.get_rules(), empty_rules);
    ASSERT_EQ(lsys.get_production_cache(), base_cache);
}

TEST(LSystemTest, set_recursion_predecessors)
{
    LSystem lsys { "F", { { 'F', "F+F" }, { 'G', "GG" } }, "F" };
    std::string expected_predecessors = "";
    std::unordered_map<int, std::pair<std::vector<int>, int>> expected_cache = { {0, {{0}, 0}} };


    lsys.set_iteration_predecessors("");
    ASSERT_EQ(lsys.get_iteration_predecessors(), expected_predecessors);
    ASSERT_EQ(lsys.get_iteration_cache(), expected_cache);
}

// Test some iterations.
TEST(LSystemTest, derivation)
{
    LSystem lsys { "F", { { 'F', "F+G" }, { 'G', "G-F" } }, "F" };

    std::string prod_iter_1 = "F+G";
//    std::string prod_iter_2 = "F+G + G-F";
    std::string prod_iter_3 = "F+G+G-F+G-F-F+G";
    
    std::vector<int> rec_iter_1 = {1, 1, 1};
//    std::vector<int> rec_iter_2 = {2,2,2, 1, 1,1,1};
    std::vector<int> rec_iter_3 = {3,3,3, 2, 2,2,2,  1,  1,1,1, 1, 2,2,2};

    auto [prod1, rec1, max1] = lsys.produce(1);
    auto [prod3, rec3, max3] = lsys.produce(3);
    
    ASSERT_EQ(prod1, prod_iter_1);
    ASSERT_EQ(rec1, rec_iter_1);
    ASSERT_EQ(max1, 1);
    ASSERT_EQ(prod3, prod_iter_3);
    ASSERT_EQ(rec3, rec_iter_3);
    ASSERT_EQ(max3, 3);
}

// Test some iterations in a non-standard order.
TEST(LSystemTest, wild_derivation)
{
    LSystem lsys { "F", { { 'F', "F+" } }, "F" };

    std::string prod_iter_1 = "F+";
    std::string prod_iter_3 = "F+++";
    std::string prod_iter_5 = "F+++++";

    std::vector<int> rec_iter_1 {1,1};
    std::vector<int> rec_iter_3 {3, 3, 2, 1};
    std::vector<int> rec_iter_5 {5, 5, 4, 3, 2, 1};

    auto [prod1, rec1, max1] = lsys.produce(1);
    auto [prod3, rec3, max3] = lsys.produce(3);
    auto [prod5, rec5, max5] = lsys.produce(5);
    
    ASSERT_EQ(prod1, prod_iter_1);
    ASSERT_EQ(rec1, rec_iter_1);
    ASSERT_EQ(max1, 1);
    ASSERT_EQ(prod3, prod_iter_3);
    ASSERT_EQ(rec3, rec_iter_3);
    ASSERT_EQ(max3, 3);
    ASSERT_EQ(prod5, prod_iter_5);
    ASSERT_EQ(rec5, rec_iter_5);
    ASSERT_EQ(max5, 5);
}

TEST(LSystemTest, disjoint_derivation)
{
    LSystem lsys { "F", { { 'F', "F+G" }, { 'G', "G-F" } }, "F" };

    lsys.produce(2);
    lsys.set_iteration_predecessors("G");
    lsys.produce(1);

    std::unordered_map<int, std::string> production_cache { {0, "F"}, {1, "F+G"}, {2, "F+G+G-F"} };
    std::unordered_map<int, std::pair<std::vector<int>, int>> recursion_cache
                    { {0, {{0}, 0}}, {1, {{0,0,0}, 0}} };

    ASSERT_EQ(lsys.get_production_cache(), production_cache);
    ASSERT_EQ(lsys.get_iteration_cache(), recursion_cache);
}

TEST(LSystemTest, serialization)
{
    LSystem olsys ("FG", { {'F', "F+G"}, {'G', "G-F" } }, "F");
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
    ASSERT_EQ(olsys.get_iteration_predecessors(), ilsys.get_iteration_predecessors());
}
