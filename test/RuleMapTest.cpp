#include <gtest/gtest.h>

#include "RuleMap.h"

TEST(RuleMapTest, default_ctor)
{
    RuleMap<int> map;
    
    RuleMap<int>::rule_map empty_rules {};
    
    ASSERT_EQ(map.get_rules(), empty_rules);
}

TEST(RuleMapTest, complete_ctor)
{
    RuleMap<int> map { { 'A', 0 } };

    RuleMap<int>::rule_map expected_rules = { { 'A', 0 } };

    ASSERT_EQ(map.get_rules(), expected_rules);
}

TEST(RuleMapTest, has_predecessor)
{
    RuleMap<int> map { { 'A', 0 } };

    ASSERT_TRUE(map.has_predecessor('A'));
    ASSERT_FALSE(map.has_predecessor('B'));
}

TEST(RuleMapTest, has_rule)
{
    RuleMap<int> map { { 'A', 0 } };

    ASSERT_TRUE(map.has_rule('A', 0));
    ASSERT_FALSE(map.has_rule('A', 1));
    ASSERT_FALSE(map.has_rule('B', 0));
}

TEST(RuleMapTest, get_rule)
{
    RuleMap<int> map { { 'A', 0 } };
    auto expected_rule = std::make_pair('A', 0);
    
    ASSERT_EQ(expected_rule, map.get_rule('A'));
}

TEST(RuleMapTest, get_rules)
{
    RuleMap<int> map { { 'A', 0 }, {'B', 1} };
    RuleMap<int>::rule_map expected_map {{'A', 0}, {'B', 1}};

    ASSERT_EQ(expected_map, map.get_rules());
}

TEST(RuleMapTest, size)
{
    RuleMap<int> map { { 'A', 0 }, {'B', 1} };

    ASSERT_EQ(2, map.size());
}

TEST(RuleMapTest, add_rule)
{
    RuleMap<int> map { } ;
    RuleMap<int>::rule_map expected_rules = { { 'A', 0 } };
    
    map.add_rule('A', 0);

    ASSERT_EQ(map.get_rules(), expected_rules);
}

TEST(RuleMapTest, remove_rule)
{
    RuleMap<int> map  { { 'A', 0 } };
    RuleMap<int>::rule_map empty_rules;

    map.remove_rule('A');

    ASSERT_EQ(map.get_rules(), empty_rules);
}

TEST(RuleMapTest, clear_rules)
{
    RuleMap<int> map { { 'A', 0 }, { 'B', 1 } };
    RuleMap<int>::rule_map empty_rules;

    map.clear_rules();

    ASSERT_EQ(map.get_rules(), empty_rules);
}

