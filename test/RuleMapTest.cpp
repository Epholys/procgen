#include <gtest/gtest.h>

#include "Observer.h"
#include "RuleMap.h"


using IntMap = RuleMap<int>;
using IntMapPtr = std::shared_ptr<IntMap>;
class IntMapObs
{
public:
    explicit IntMapObs(IntMapPtr ptr)
        : obs_(ptr)
        {
            obs_.add_callback([this](){notified_ = true;});
        }

    explicit operator bool() const
        {
            return notified_;
        }

private:
    Observer<IntMap> obs_ {nullptr};
    bool notified_ {false};
};



TEST(RuleMapTest, default_ctor)
{
    IntMap map {};

    IntMap::Rules empty_rules {};

    ASSERT_EQ(map.get_rules(), empty_rules);
}

TEST(RuleMapTest, complete_ctor)
{
    IntMap map { { 'A', 0 } };

    IntMap::Rules expected_rules = { { 'A', 0 } };

    ASSERT_EQ(map.get_rules(), expected_rules);
}

// TODO: operator= ; ctor(other)

TEST(RuleMapTest, has_predecessor)
{
    IntMap map { { 'A', 0 } };

    ASSERT_TRUE(map.has_predecessor('A'));
    ASSERT_FALSE(map.has_predecessor('B'));
}

TEST(RuleMapTest, has_rule)
{
    IntMap map { { 'A', 0 } };

    ASSERT_TRUE(map.has_rule('A', 0));
    ASSERT_FALSE(map.has_rule('A', 1));
    ASSERT_FALSE(map.has_rule('B', 0));
}

TEST(RuleMapTest, get_rule)
{
    IntMap map { { 'A', 0 } };
    auto expected_rule = std::make_pair('A', 0);

    ASSERT_EQ(expected_rule, map.get_rule('A'));
}

TEST(RuleMapTest, get_rules)
{
    IntMap map { { 'A', 0 }, {'B', 1} };
    IntMap::Rules expected_map {{'A', 0}, {'B', 1}};

    ASSERT_EQ(expected_map, map.get_rules());
}

TEST(RuleMapTest, size)
{
    IntMap map { { 'A', 0 }, {'B', 1} };

    ASSERT_EQ(2, map.size());
}

TEST(RuleMapTest, add_rule)
{
    IntMapPtr map = std::make_shared<IntMap>();
    IntMapObs obs (map);

    IntMap::Rules expected_rules = { { 'A', 0 } };

    map->add_rule('A', 0);

    ASSERT_EQ(map->get_rules(), expected_rules);
    ASSERT_TRUE(obs);
}

TEST(RuleMapTest, remove_rule)
{
    IntMapPtr map = std::make_shared<IntMap>(IntMap::Rules({ { 'A', 0 } }));
    IntMapObs obs (map);
    IntMap::Rules empty_rules;

    map->remove_rule('A');

    ASSERT_EQ(map->get_rules(), empty_rules);
    ASSERT_TRUE(obs);
}

TEST(RuleMapTest, clear_rules)
{
    IntMapPtr map = std::make_shared<IntMap>(IntMap::Rules({ { 'A', 0 }, { 'B', 1 } }));
    IntMapObs obs (map);
    IntMap::Rules empty_rules;

    map->clear_rules();

    ASSERT_EQ(map->get_rules(), empty_rules);
    ASSERT_TRUE(obs);
}

TEST(RuleMapTest, replace_rules)
{
    IntMapPtr map = std::make_shared<IntMap>(IntMap::Rules({ { 'A', 0 }, { 'B', 1 } }));
    IntMapObs obs (map);
    IntMap::Rules new_rules {{'C', 2}, {'D', 3}};

    map->replace_rules(new_rules);

    ASSERT_EQ(new_rules, map->get_rules());
    ASSERT_TRUE(obs);
}
