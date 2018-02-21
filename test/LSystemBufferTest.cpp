#include <iostream>

#include <gtest/gtest.h>

#include "LSystemBuffer.h"
#include "helper_algorithm.h"

using namespace procgui;

// Sync is tested in each unit test.
class LSystemBufferTest :  public ::testing::Test
{
public:
    using validity = LSystemBuffer::validity;
    using predecessor = LSystemBuffer::predecessor;
    using successor = LSystemBuffer::successor;
    using rule = LSystemBuffer::rule;
    using const_iterator = LSystemBuffer::const_iterator;

    LSystemBufferTest()
        : buffer1 {lsys}
        , buffer2 {lsys}
        {
        }

    bool has_predecessor (const LSystemBuffer& buff, predecessor pred) const
        {
            for (auto tuple : buff)
            {
                if (std::get<predecessor>(tuple) == pred)
                {
                    return true;
                }
            }
            return false;
        }

    bool has_rule (const LSystemBuffer& buff, predecessor pred, successor succ) const
        {
            for (auto tuple : buff)
            {
                if (std::get<predecessor>(tuple) == pred &&
                    std::get<successor>(tuple) == succ)
                {
                    return true;
                }
            }
            return false;
        }

    bool has_duplicate(const LSystemBuffer& buff, const_iterator it) const
        {
            for (auto jt = buff.begin(); jt != buff.end(); ++jt)
            {
                if (it != jt &&
                    std::get<predecessor>(*it) == std::get<predecessor>(*jt))
                {
                    return true;
                }
            }
            return false;
        }

    bool duplicates_marked(const LSystemBuffer& buff) const
        {
            for(auto it = buff.begin(); it != buff.end(); ++it)
            {
                auto jt = it;
                while ((jt = find_duplicate(it, jt, buff.end())) != buff.end())
                {
                    if (std::get<validity>(*jt))
                    {
                        return false;
                    }
                }
            }
            return true;
        }
    
    size_t buffer_size(const LSystemBuffer& buff) const
        {
            size_t n = 0;
            for(auto t : buff)
            {
                n++;
            }
            return n;
        }

    LSystem::production_rules rules { { 'F', "G-F-G" }, { 'G', "F+G+F" } } ;
    std::shared_ptr<LSystem> lsys = std::make_shared<LSystem>("F", rules);
    LSystemBuffer buffer1;
    LSystemBuffer buffer2;
};

TEST_F(LSystemBufferTest, helper_has_predecessor)
{
    ASSERT_TRUE(has_predecessor(buffer1, 'F'));
    ASSERT_FALSE(has_predecessor(buffer1, 'X'));
}

TEST_F(LSystemBufferTest, helper_has_rule)
{
    ASSERT_TRUE(has_rule(buffer1, 'F', "G-F-G"));
    ASSERT_FALSE(has_rule(buffer1, 'F', "AAA"));
}

TEST_F(LSystemBufferTest, helper_has_duplicate)
{
    ASSERT_FALSE(has_duplicate(buffer1, buffer1.begin()));

    auto pred = std::get<predecessor>(*buffer1.begin());
    buffer1.add_rule();
    buffer1.change_predecessor(std::prev(buffer1.end()), pred);
    ASSERT_TRUE(has_duplicate(buffer1, buffer1.begin()));
}

TEST_F(LSystemBufferTest, helper_size)
{
    ASSERT_EQ(2, buffer_size(buffer1));
}

TEST_F(LSystemBufferTest, constructor)
{
    // Check if all lsys' rules are here
    for (auto it = lsys->get_rules().begin();
         it != lsys->get_rules().end();
         ++it)
    {
        ASSERT_TRUE(has_rule(buffer1, it->first, it->second));
    }

    // no sync
}

TEST_F(LSystemBufferTest, add_rule)
{
    buffer1.add_rule();

    // Check if the last rule is an empty one
    ASSERT_EQ(*std::prev(buffer1.end()), rule({true, '\0', ""}));

    ASSERT_NE(*std::prev(buffer2.end()), rule({true, '\0', ""}));
}

TEST_F(LSystemBufferTest, change_predecessor_simple)
{
    auto it = buffer1.begin();
    auto old_pred = std::get<predecessor>(*it);
    buffer1.change_predecessor(it, 'A');
    
    ASSERT_TRUE(has_predecessor(buffer1, 'A'));
    ASSERT_FALSE(has_predecessor(buffer1, old_pred));
    
    ASSERT_TRUE(has_predecessor(buffer2, 'A'));
    ASSERT_FALSE(has_predecessor(buffer2, old_pred));
}

TEST_F(LSystemBufferTest, change_predecessor_duplicate)
{
    auto first_pred = std::get<predecessor>(*buffer1.begin());

    buffer1.add_rule();
    auto it = std::prev(buffer1.end());
    buffer1.change_predecessor(it, first_pred);
    buffer1.change_successor(it, "AAA");
    buffer1.change_predecessor(it, 'A');

    // TODO
    ASSERT_TRUE(has_duplicate(buffer1, buffer1.begin()));
    ASSERT_TRUE(duplicates_marked(buffer1));
    
    ASSERT_FALSE(has_duplicate(buffer2, buffer2.begin()));
}


TEST_F(LSystemBufferTest, remove_predecessor_simple)
{
    auto it = buffer1.begin();
    auto pred = std::get<predecessor>(*it);
    buffer1.remove_predecessor(it);

    ASSERT_THROW(lsys->get_rule(pred), gsl::fail_fast);

    ASSERT_FALSE(has_predecessor(buffer1, pred));
    
    ASSERT_FALSE(has_predecessor(buffer2, pred));
}

TEST_F(LSystemBufferTest, remove_predecessor_duplicate)
{
    auto begin = buffer1.begin();
    auto pred = std::get<predecessor>(*begin);

    buffer1.add_rule();

    auto end = std::prev(buffer1.end());
    buffer1.change_predecessor(end, pred);
    buffer1.change_successor(end, "AAA");
    buffer1.remove_predecessor(begin);

    ASSERT_EQ("AAA", lsys->get_rule(pred).second);

    ASSERT_FALSE(has_duplicate(buffer1, buffer1.begin()));

    ASSERT_FALSE(has_duplicate(buffer2, buffer2.begin()));
}

TEST_F(LSystemBufferTest, change_successor_simple)
{
    buffer1.change_successor(buffer1.begin(), "AAA");

    auto pred = std::get<predecessor>(*buffer1.begin());
    ASSERT_EQ("AAA", lsys->get_rule(pred).second);

    ASSERT_EQ("AAA", std::get<successor>(*buffer1.begin()));

    ASSERT_EQ("AAA", std::get<successor>(*buffer2.begin()));
}

TEST_F(LSystemBufferTest, change_successor_duplicate)
{
    auto first_pred = std::get<predecessor>(*buffer1.begin());

    buffer1.add_rule();
    
    auto last_rule = std::prev(buffer1.end());
    buffer1.change_predecessor(last_rule, first_pred);
    buffer1.change_successor(last_rule, "AAA");

    ASSERT_NE("AAA", lsys->get_rule(first_pred).second);
    ASSERT_EQ("AAA", std::get<successor>(*last_rule));
    ASSERT_NE("AAA", std::get<successor>(*buffer1.begin()));
    ASSERT_NE("AAA", std::get<successor>(*buffer2.begin()));
}

TEST_F(LSystemBufferTest, erase_simple)
{
    auto pred = std::get<predecessor>(*buffer1.begin());
    buffer1.erase(buffer1.begin());

    ASSERT_THROW(lsys->get_rule(pred), gsl::fail_fast);
    ASSERT_FALSE(has_predecessor(buffer1, pred));
    ASSERT_FALSE(has_predecessor(buffer2, pred));
}

TEST_F(LSystemBufferTest, erase_duplicate)
{
    auto first_pred = std::get<predecessor>(*buffer1.begin());

    buffer1.add_rule();

    auto last_rule = std::prev(buffer1.end());
    buffer1.change_predecessor(last_rule, first_pred);
    buffer1.erase(last_rule);

    ASSERT_NO_THROW(lsys->get_rule(first_pred));

    ASSERT_TRUE(has_predecessor(buffer1, first_pred));
    ASSERT_FALSE(has_duplicate(buffer1, buffer1.begin()));

    ASSERT_TRUE(has_predecessor(buffer2, first_pred));
    ASSERT_FALSE(has_duplicate(buffer2, buffer2.begin()));
}

TEST_F(LSystemBufferTest, erase_empty)
{
    auto size = buffer_size(buffer1);
    buffer1.add_rule();
    buffer1.erase(std::prev(buffer1.end()));
    
    ASSERT_EQ(size, buffer_size(buffer1));
}

TEST_F(LSystemBufferTest, erase_replacement)
{
    auto pred = std::get<predecessor>(*buffer1.begin());
    auto first_succ = std::get<successor>(*buffer1.begin());
    auto next_succ = "AAA";
    
    buffer1.add_rule();
    
    auto last_rule = std::prev(buffer1.end());
    buffer1.change_predecessor(last_rule, pred);
    buffer1.change_successor(last_rule, next_succ);
    buffer1.erase(buffer1.begin());

    ASSERT_EQ(next_succ, lsys->get_rule(pred).second);
    ASSERT_TRUE(has_rule(buffer1, pred, next_succ));
    ASSERT_TRUE(has_rule(buffer2, pred, next_succ));
}

TEST_F(LSystemBufferTest, advanced_sync_and_layout)
{
    // buffer1:    // buffer2:    
    // F -> G-F-G  // F -> G-F-G
    // G -> F+G+F  // G -> F+G+F
    
    buffer1.add_rule();

    auto last_rule = std::prev(buffer1.end());
    buffer1.change_predecessor(last_rule, 'X');
    buffer1.change_successor(last_rule, "XXX");

    buffer1.add_rule();
    last_rule = std::prev(buffer1.end());
    buffer1.change_predecessor(last_rule, 'X');
    buffer1.change_successor(last_rule, "YYY");
    
    // buffer1:        // buffer2:    
    // F -> G-F-G      // F -> G-F-G
    // G -> F+G+F      // G -> F+G+F
    // X -> XXX        // X -> XXX
    // X -> YYY (dupe) // 

    last_rule = std::prev(buffer2.end());
    buffer2.erase(last_rule);

    // buffer1:        // buffer2:    
    // F -> G-F-G      // F -> G-F-G
    // G -> F+G+F      // G -> F+G+F
    // X -> YYY        // X -> YYY
  
    ASSERT_EQ("YYY", lsys->get_rule('X').second);
    ASSERT_TRUE(has_predecessor(buffer1, 'X'));
    ASSERT_FALSE(has_duplicate(buffer1, std::prev(buffer1.end())));
    ASSERT_TRUE(has_predecessor(buffer2, 'X'));
    ASSERT_FALSE(has_duplicate(buffer2, std::prev(buffer2.end())));
}
