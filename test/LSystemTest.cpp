#include <sstream>
#include <gtest/gtest.h>
#include "cereal/archives/json.hpp"
#include "Observer.h"
#include "LSystem.h"


using Rules = LSystem::Rules;
using ProductionCache = std::unordered_map<std::uint8_t, std::string>;
using IterationCache = std::unordered_map<std::uint8_t, std::pair<std::vector<std::uint8_t>, std::uint8_t>>;

// Check if all the members are initialized. Not so useful by itself, except if
// one of the members suddenly does not have a default initialization.
TEST(LSystemTest, default_ctor)
{
    LSystem lsys;

    LSystem::Rules empty_rules;
    std::string empty_str;
    ProductionCache empty_prod_cache;
    IterationCache empty_rec_cache;

    ASSERT_EQ(lsys.get_axiom(), empty_str);
    ASSERT_EQ(lsys.get_rules(), empty_rules);
    ASSERT_EQ(lsys.get_production_cache(), empty_prod_cache);
    ASSERT_EQ(lsys.get_iteration_predecessors(), empty_str);
    ASSERT_EQ(lsys.get_iteration_cache(), empty_rec_cache);
}

TEST(LSystemTest, complete_ctor)
{
    LSystem lsys { "F", { { 'F', "F+F" } }, "F" };
    LSystem::Rules expected_rules = { { 'F', "F+F" } };
    IterationCache expected_iteration_cache = { {0, {{0}, 0}} };

    ASSERT_EQ(lsys.get_axiom(), "F");
    ASSERT_EQ(lsys.get_rules(), expected_rules);
    ASSERT_EQ(lsys.get_production_cache().at(0), "F");
    ASSERT_EQ(lsys.get_iteration_predecessors(), "F");
    ASSERT_EQ(lsys.get_iteration_cache(), expected_iteration_cache);
}
// Other constructors are defaulted, we assume the implementation is correct.

TEST(LSystemTest, get_axiom)
{
    LSystem lsys { "F", { { 'F', "F+F" } }, "F"  };

    ASSERT_EQ(lsys.get_axiom(), "F");
}

TEST(LSystemTest, set_axiom)
{
    LSystem lsys ("F", Rules({{'F', "F+F"}}), "F");
    ProductionCache axiom_prod_cache { { 0, "FF" } };
    IterationCache axiom_iter_cache { {0, {{0, 0}, 0 }} };

    lsys.set_axiom("FF");

    ASSERT_EQ(lsys.get_axiom(), "FF");
    ASSERT_EQ(lsys.get_production_cache(), axiom_prod_cache);
    ASSERT_EQ(lsys.get_iteration_cache(), axiom_iter_cache);

    ASSERT_TRUE(lsys.poll_modification());
}

TEST(LSystemTest, add_rule)
{
    LSystem lsys ("F", Rules({ }), "F");
    LSystem::Rules expected_rules = { { 'F', "F+F" } };
    ProductionCache base_prod_cache { { 0, "F" } };
    IterationCache base_iter_cache { {0, {{0}, 0 }} };
    lsys.add_rule('F', "F+F");

    ASSERT_EQ(lsys.get_rules(), expected_rules);
    ASSERT_EQ(lsys.get_production_cache(), base_prod_cache);
    ASSERT_EQ(lsys.get_iteration_cache(), base_iter_cache);
    ASSERT_TRUE(lsys.poll_modification());
}

TEST(LSystemTest, remove_rule)
{
    LSystem lsys ("F", Rules({ { 'F', "F+F" } }), "F");
    LSystem::Rules empty_rules;
    ProductionCache base_prod_cache { { 0, "F" } };
    IterationCache base_iter_cache { {0, {{0}, 0 }} };

    lsys.remove_rule('F');

    ASSERT_EQ(lsys.get_rules(), empty_rules);
    ASSERT_EQ(lsys.get_production_cache(), base_prod_cache);
    ASSERT_EQ(lsys.get_iteration_cache(), base_iter_cache);

    ASSERT_TRUE(lsys.poll_modification());

    ASSERT_THROW(lsys.remove_rule('G'), gsl::fail_fast);
}

TEST(LSystemTest, clear_rules)
{
    LSystem lsys ("F", Rules({ { 'F', "F+F" }, { 'G', "GG" } }), "F");
    LSystem::Rules empty_rules;
    ProductionCache base_cache { { 0, "F" } };

    lsys.clear_rules();

    ASSERT_EQ(lsys.get_rules(), empty_rules);
    ASSERT_EQ(lsys.get_production_cache(), base_cache);

    ASSERT_TRUE(lsys.poll_modification());
}

TEST(LSystemTest, replace_rules)
{
    LSystem lsys ("F", Rules({ { 'F', "FF" }, { 'G', "GG" } }), "F");
    LSystem::Rules new_rules {{'H', "HH"},{'I', "II"}};;
    ProductionCache base_prod_cache { { 0, "F" } };
    IterationCache base_iter_cache { {0, {{0}, 0 }} };

    lsys.replace_rules(new_rules);

    ASSERT_EQ(lsys.get_rules(), new_rules);
    ASSERT_EQ(lsys.get_production_cache(), base_prod_cache);
    ASSERT_EQ(lsys.get_iteration_cache(), base_iter_cache);

    ASSERT_TRUE(lsys.poll_modification());
}

TEST(LSystemTest, set_iteration_predecessors)
{
    LSystem lsys("F", Rules({ { 'F', "F+F" }, { 'G', "GG" } }), "F");
    std::string expected_predecessors = "";
    IterationCache expected_cache = { {0, {{0}, 0}} };


    lsys.set_iteration_predecessors("");
    ASSERT_EQ(lsys.get_iteration_predecessors(), expected_predecessors);
    ASSERT_EQ(lsys.get_iteration_cache(), expected_cache);

    ASSERT_TRUE(lsys.poll_modification());
}

// Test some iterations.
TEST(LSystemTest, derivation)
{
    LSystem lsys { "F", { { 'F', "F+G" }, { 'G', "G-F" } }, "F" };

    std::string prod_iter_1 = "F+G";
//    std::string prod_iter_2 = "F+G + G-F";
    std::string prod_iter_3 = "F+G+G-F+G-F-F+G";

    std::vector<std::uint8_t> rec_iter_1 = {1, 1, 1};
//    std::vector<std::uint8_t> rec_iter_2 = {2,2,2, 1, 1,1,1};
    std::vector<std::uint8_t> rec_iter_3 = {3,3,3, 2, 2,2,2,  1,  1,1,1, 1, 2,2,2};

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

    std::vector<std::uint8_t> rec_iter_1 {1,1};
    std::vector<std::uint8_t> rec_iter_3 {3, 3, 2, 1};
    std::vector<std::uint8_t> rec_iter_5 {5, 5, 4, 3, 2, 1};

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


// Test if a default rule is correctly iterated
TEST(LSystemTest, corner_iteration)
{
    LSystem lsys { "FG", { { 'F', "FG" } }, "G" };

    std::string prod_iter_3 = "FGGGG";

    std::vector<std::uint8_t> rec_iter_3 {0, 0, 1, 2, 3};

    auto [prod3, rec3, max3] = lsys.produce(3);

    ASSERT_EQ(rec_iter_3, rec3);
    ASSERT_EQ(3, max3);
}

TEST(LSystemTest, disjoint_derivation)
{
    LSystem lsys { "F", { { 'F', "F+G" }, { 'G', "G-F" } }, "F" };

    lsys.produce(2);
    lsys.set_iteration_predecessors("G");
    lsys.produce(1);

    ProductionCache production_cache { {0, "F"}, {1, "F+G"}, {2, "F+G+G-F"} };
    IterationCache iteration_cache
                    { {0, {{0}, 0}}, {1, {{0,0,0}, 0}} };

    ASSERT_EQ(lsys.get_production_cache(), production_cache);
    ASSERT_EQ(lsys.get_iteration_cache(), iteration_cache);
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
