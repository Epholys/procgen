#include <iostream>

#include <gtest/gtest.h>

#include "LSystemBuffer.h"
#include "helper_algorithm.h"

using namespace procgui;

// Sync is tested in each unit test.
class LSystemBufferTest :  public ::testing::Test
{
public:
    using const_iterator = LSystemBuffer::const_iterator;
    using successor = LSystemBuffer::succ;
    using rule = LSystemBuffer::Rule;

    LSystemBufferTest()
        : buffer1 {lsys}
        , buffer2 {lsys}
        {
        }

    bool has_predecessor (const LSystemBuffer& buff, char pred) const
        {
            for (auto rule : buff)
            {
                if (rule.predecessor == pred)
                {
                    return true;
                }
            }
            return false;
        }

    bool has_rule (const LSystemBuffer& buff, char pred, const std::string& succ) const
        {
            for (auto rule : buff)
            {
                if (rule.predecessor == pred &&
                    rule.successor == succ)
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
                    it->predecessor == jt->predecessor)
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
                    if (jt->validity)
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

        
    const char pred1 = 'A';
    const char pred2 = 'B';
    const std::string succ1 = "AAA";
    const std::string succ2 = "BBB";
    LSystem::production_rules rules { { pred1, succ1 }, { pred2, succ2 } } ;
    std::shared_ptr<LSystem> lsys = std::make_shared<LSystem>("", rules);
    LSystemBuffer buffer1;
    LSystemBuffer buffer2;
    const char new_pred1 = 'X';
    const char new_pred2 = 'Y';
    const std::string new_succ1 = "XXX";
    const std::string new_succ2 = "YYY";
    const std::string new_succ3 = "ZZZ";
};

TEST_F(LSystemBufferTest, helper_has_predecessor)
{
    ASSERT_TRUE(has_predecessor(buffer1, pred1));
    ASSERT_FALSE(has_predecessor(buffer1, new_pred1));
}

TEST_F(LSystemBufferTest, helper_has_rule)
{
    ASSERT_TRUE(has_rule(buffer1, pred1, succ1));
    ASSERT_FALSE(has_rule(buffer1, pred1, new_succ1));
    ASSERT_FALSE(has_rule(buffer1, new_pred1, new_succ1));
}

TEST_F(LSystemBufferTest, helper_has_duplicate)
{
    ASSERT_FALSE(has_duplicate(buffer1, buffer1.begin()));

    auto first_pred = buffer1.begin()->predecessor;
    buffer1.add_rule();
    buffer1.change_predecessor(std::prev(buffer1.end()), first_pred);
    ASSERT_TRUE(has_duplicate(buffer1, buffer1.begin()));
    ASSERT_TRUE(duplicates_marked(buffer1));
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
    auto begin = buffer1.begin();
    auto old_pred = begin->predecessor;
    auto old_succ = begin->successor;
    buffer1.change_predecessor(begin, new_pred1);

    ASSERT_TRUE(lsys->has_rule(new_pred1, old_succ));
    ASSERT_FALSE(lsys->has_rule(old_pred, old_succ));
    
    ASSERT_TRUE(has_predecessor(buffer1, new_pred1));
    ASSERT_FALSE(has_predecessor(buffer1, old_pred));
    
    ASSERT_TRUE(has_predecessor(buffer2, new_pred1));
    ASSERT_FALSE(has_predecessor(buffer2, old_pred));
}

TEST_F(LSystemBufferTest, change_predecessor_is_duplicated)
{
    auto begin = buffer1.begin();
    auto first_pred = begin->predecessor;
    auto first_succ = begin->successor;

    buffer1.add_rule();

    auto end = std::prev(buffer1.end());
    buffer1.change_successor(end, new_succ1);
    buffer1.change_predecessor(end, first_pred);

    ASSERT_TRUE(lsys->has_rule(first_pred, first_succ));
    
    ASSERT_TRUE(has_rule(buffer1, first_pred, new_succ1));
    ASSERT_TRUE(has_duplicate(buffer1, begin));
    ASSERT_TRUE(duplicates_marked(buffer1));

    ASSERT_FALSE(has_rule(buffer2, first_pred, new_succ1));
}

TEST_F(LSystemBufferTest, change_predecessor_remove_rule)
{
    auto begin = buffer1.begin();
    auto first_pred = begin->predecessor;
    auto first_succ = begin->successor;
    
    buffer1.change_predecessor(begin, new_pred1);

    ASSERT_TRUE(lsys->has_rule(new_pred1, first_succ));
    ASSERT_FALSE(lsys->has_rule(first_pred, first_succ));

    ASSERT_TRUE(has_rule(buffer1, new_pred1, first_succ));
    ASSERT_FALSE(has_rule(buffer1, first_pred, first_succ));

    ASSERT_TRUE(has_rule(buffer2, new_pred1, first_succ));
    ASSERT_FALSE(has_rule(buffer2, first_pred, first_succ));
}

TEST_F(LSystemBufferTest, change_predecessor_remove_rule_duplicated)
{
    auto begin = buffer1.begin();
    auto first_pred = begin->predecessor;
    auto first_succ = begin->successor;
    buffer1.add_rule();
    
    auto end = std::prev(buffer1.end());
    buffer1.change_predecessor(end, first_pred);
    buffer1.change_successor(end, new_succ1);

    buffer1.change_predecessor(end, new_pred1);
   
    // The first rule was not removed
    ASSERT_TRUE(lsys->has_rule(first_pred, first_succ));
    ASSERT_TRUE(lsys->has_rule(new_pred1, new_succ1));

    ASSERT_TRUE(has_rule(buffer2, first_pred, first_succ));
    ASSERT_TRUE(has_rule(buffer2, new_pred1, new_succ1));
}

TEST_F(LSystemBufferTest, change_predecessor_double_duplication)
{
    buffer1.add_rule();
    auto end = std::prev(buffer1.end());
    buffer1.change_predecessor(end, new_pred1);
    buffer1.change_successor(end, new_succ1);

    buffer1.add_rule();
    end = std::prev(buffer1.end());
    buffer1.change_predecessor(end, new_pred1);
    buffer1.change_successor(end, new_succ2);

    buffer1.add_rule();
    end = std::prev(buffer1.end());
    buffer1.change_predecessor(end, new_pred1);
    buffer1.change_successor(end, new_succ3);

    // buffer1:
    // pred1     -> succ1
    // pred2     -> succ2
    // new_pred1 -> new_succ1
    //         1 ->         2 (duplicate) <-to_change
    //         1 ->         3 (duplicate)

    auto to_change = std::prev(std::prev(buffer1.end()));
    buffer1.change_predecessor(to_change, new_pred2);

    // buffer1:
    // pred1     -> succ1
    // pred2     -> succ2
    // new_pred1 -> new_succ1
    //         2 ->         2 
    //         1 ->         3 (duplicate)

    buffer1.change_predecessor(to_change, new_pred1);

    // buffer1 (expected):
    // pred1     -> succ1
    // pred2     -> succ2
    // new_pred1 -> new_succ1
    //         1 ->         2 (duplicate)
    //         1 ->         3 (duplicate)

    ASSERT_TRUE(lsys->has_rule(new_pred1, new_succ1));
    ASSERT_FALSE(lsys->has_predecessor(new_pred2));

    ASSERT_TRUE(has_rule(buffer1, new_pred1, new_succ1));
    ASSERT_FALSE(has_predecessor(buffer1, new_pred2));
    ASSERT_TRUE(duplicates_marked(buffer1));

    ASSERT_TRUE(has_rule(buffer2, new_pred1, new_succ1));
    ASSERT_FALSE(has_predecessor(buffer2, new_pred2));
}

TEST_F(LSystemBufferTest, change_predecessor_duplication_before_after)
{
    auto begin = buffer1.begin();
    auto first_pred = begin->predecessor;
    auto first_succ = begin->successor;

    buffer1.add_rule();
    auto end = std::prev(buffer1.end());
    buffer1.change_predecessor(end, new_pred1);
    buffer1.change_successor(end, new_succ1);

    buffer1.add_rule();
    end = std::prev(buffer1.end());
    buffer1.change_predecessor(end, new_pred1);
    buffer1.change_successor(end, new_succ2);

    // buffer1:
    // A -> AAA
    // B -> BBB
    // X -> XXX <= to_change
    // X -> YYY (duplicate)
    auto to_change = std::prev(std::prev(buffer1.end()));
    buffer1.change_predecessor(to_change, first_pred);

    // buffer1:
    // A -> AAA
    // B -> BBB
    // A -> XXX (duplicate)
    // X -> YYY
    ASSERT_TRUE(lsys->has_rule(first_pred, first_succ));
    ASSERT_TRUE(lsys->has_rule(new_pred1, new_succ2));
    ASSERT_TRUE(has_rule(buffer1, first_pred, first_succ));
    ASSERT_TRUE(has_rule(buffer1, new_pred1, new_succ2));
    ASSERT_TRUE(has_duplicate(buffer1, begin));
    ASSERT_TRUE(duplicates_marked(buffer1));
    ASSERT_TRUE(has_rule(buffer2, first_pred, first_succ));
    ASSERT_TRUE(has_rule(buffer2, new_pred1, new_succ2));
}

TEST_F(LSystemBufferTest, change_predecessor_duplication_go_back)
{
    auto begin = buffer1.begin();
    auto first_pred = begin->predecessor;
    auto first_succ = begin->successor;

    buffer1.add_rule();
    auto end = std::prev(buffer1.end());
    buffer1.change_predecessor(end, new_pred1);
    buffer1.change_successor(end, new_succ1);

    buffer1.add_rule();
    end = std::prev(buffer1.end());
    buffer1.change_predecessor(end, new_pred1);
    buffer1.change_successor(end, new_succ2);

    // buffer1:
    // A -> AAA
    // B -> BBB
    // X -> XXX <= to_change
    // X -> YYY (duplicate)
    auto to_change = std::prev(std::prev(buffer1.end()));
    buffer1.change_predecessor(to_change, first_pred);

    // buffer1:
    // A -> AAA
    // B -> BBB
    // A -> XXX (duplicate)
    // X -> YYY

    buffer1.change_predecessor(to_change, new_pred1);
    // buffer1:
    // A -> AAA
    // B -> BBB
    // X -> XXX (duplicate)
    // X -> YYY

    ASSERT_TRUE(lsys->has_rule(first_pred, first_succ));
    ASSERT_TRUE(lsys->has_rule(new_pred1, new_succ2));
    ASSERT_TRUE(has_rule(buffer1, first_pred, first_succ));
    ASSERT_TRUE(has_rule(buffer1, new_pred1, new_succ2));
    ASSERT_TRUE(has_duplicate(buffer1, std::prev(buffer1.end())));
    ASSERT_TRUE(duplicates_marked(buffer1));
    ASSERT_TRUE(has_rule(buffer2, first_pred, first_succ));
    ASSERT_TRUE(has_rule(buffer2, new_pred1, new_succ2));
}



TEST_F(LSystemBufferTest, remove_predecessor_simple)
{
    auto begin = buffer1.begin();
    auto pred = begin->predecessor;
    buffer1.remove_predecessor(begin);

    ASSERT_FALSE(lsys->has_predecessor(pred));

    ASSERT_FALSE(has_predecessor(buffer1, pred));
    
    ASSERT_FALSE(has_predecessor(buffer2, pred));

    // There is a scratch buffer
    ASSERT_TRUE(buffer_size(buffer1) == buffer_size(buffer2) + 1);
}

TEST_F(LSystemBufferTest, remove_predecessor_duplicate)
{
    auto begin = buffer1.begin();
    auto first_pred = begin->predecessor;

    buffer1.add_rule();

    auto end = std::prev(buffer1.end());
    buffer1.change_predecessor(end, first_pred);
    buffer1.change_successor(end, new_succ1);
    buffer1.remove_predecessor(begin);

    ASSERT_TRUE(lsys->has_rule(first_pred, new_succ1));
    ASSERT_TRUE(has_rule(buffer1, first_pred, new_succ1));
    ASSERT_TRUE(has_rule(buffer2, first_pred, new_succ1));
}

TEST_F(LSystemBufferTest, change_successor_simple)
{
    buffer1.change_successor(buffer1.begin(), new_succ1);

    auto pred = buffer1.begin()->predecessor;

    ASSERT_TRUE(lsys->has_rule(pred, new_succ1));
    ASSERT_TRUE(lsys->has_rule(pred, new_succ1));
    ASSERT_TRUE(lsys->has_rule(pred, new_succ1));
}

TEST_F(LSystemBufferTest, change_successor_duplicate)
{
    auto begin = buffer1.begin();
    auto first_pred = begin->predecessor;
    auto first_succ = begin->successor;

    buffer1.add_rule();
    
    auto end = std::prev(buffer1.end());
    buffer1.change_predecessor(end, first_pred);
    buffer1.change_successor(end, new_succ1);

    ASSERT_FALSE(lsys->has_rule(first_pred, new_succ1));

    ASSERT_TRUE(has_rule(buffer1, first_pred, first_succ));
    ASSERT_TRUE(has_rule(buffer1, first_pred, new_succ1));
    ASSERT_TRUE(duplicates_marked(buffer1));

    ASSERT_FALSE(has_rule(buffer2, first_pred, new_succ1));
}

TEST_F(LSystemBufferTest, erase_simple)
{
    auto size = buffer_size(buffer1);
    auto pred = buffer1.begin()->predecessor;
    buffer1.erase(buffer1.begin());

    ASSERT_FALSE(lsys->has_predecessor(pred));
    ASSERT_FALSE(has_predecessor(buffer1, pred));
    ASSERT_EQ(size-1, buffer_size(buffer1));
    ASSERT_FALSE(has_predecessor(buffer2, pred));
    ASSERT_EQ(size-1, buffer_size(buffer2));
}

TEST_F(LSystemBufferTest, erase_duplicate)
{
    auto size = buffer_size(buffer1);
    auto begin = buffer1.begin();
    auto first_pred = begin->predecessor;
    auto first_succ = begin->successor;

    buffer1.add_rule();

    auto end = std::prev(buffer1.end());
    buffer1.change_predecessor(end, first_pred);
    buffer1.erase(end);

    ASSERT_TRUE(lsys->has_rule(first_pred, first_succ));

    ASSERT_TRUE(has_predecessor(buffer1, first_pred));
    ASSERT_FALSE(has_duplicate(buffer1, buffer1.begin()));
    ASSERT_EQ(size, buffer_size(buffer1));

    ASSERT_TRUE(has_predecessor(buffer2, first_pred));
    ASSERT_FALSE(has_duplicate(buffer2, buffer2.begin()));
    ASSERT_EQ(size, buffer_size(buffer1));
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
    auto size = buffer_size(buffer1);
    auto begin  = buffer1.begin();
    auto first_pred = begin->predecessor;
    auto first_succ = begin->successor;
    
    buffer1.add_rule();
    
    auto end = std::prev(buffer1.end());
    buffer1.change_predecessor(end, first_pred);
    buffer1.change_successor(end, new_succ1);
    buffer1.erase(begin);

    ASSERT_TRUE(lsys->has_rule(first_pred, new_succ1));
    
    ASSERT_TRUE(has_rule(buffer1, first_pred, new_succ1));
    ASSERT_EQ(size, buffer_size(buffer1));

    ASSERT_TRUE(has_rule(buffer2, first_pred, new_succ1));
    ASSERT_EQ(size, buffer_size(buffer2));
}

TEST_F(LSystemBufferTest, advanced_sync_and_layout1)
{
    // Example:
    // buffer1:    // buffer2:    
    // A -> AAA    // A -> AAA
    // B -> BBB    // B -> BBB
    
    buffer1.add_rule();
    auto end = std::prev(buffer1.end());
    buffer1.change_predecessor(end, new_pred1);
    buffer1.change_successor(end, new_succ1);

    buffer1.add_rule();
    end = std::prev(buffer1.end());
    buffer1.change_predecessor(end, new_pred1);
    buffer1.change_successor(end, new_succ2);
    
    // buffer1:        // buffer2:    
    // A -> AAA        // A -> AAA
    // B -> BBB        // B -> BBB
    // X -> XXX        // X -> XXX
    // X -> YYY (dupe) // 

    end = std::prev(buffer2.end());
    buffer2.erase(end);

    // buffer1:        // buffer2:    
    // A -> AAA        // A -> AAA
    // B -> BBB        // B -> BBB
    // X -> YYY        // X -> YYY
  
    ASSERT_TRUE(lsys->has_rule(new_pred1, new_succ2));
    ASSERT_TRUE(has_predecessor(buffer1, new_pred1));
    ASSERT_FALSE(has_duplicate(buffer1, std::prev(buffer1.end())));
    ASSERT_TRUE(has_predecessor(buffer2, new_pred1));
    ASSERT_FALSE(has_duplicate(buffer2, std::prev(buffer2.end())));
}

// Synchronize in priority the window in which we remove a rule.
TEST_F(LSystemBufferTest, advanced_sync_and_layout2)
{
    // Example:
    // buffer1:    // buffer2:    
    // A -> AAA    // A -> AAA
    // B -> BBB    // B -> BBB
    
    auto first_pred = buffer1.begin()->predecessor;
    auto first_succ = buffer1.begin()->successor;
    buffer2.add_rule();
    auto end = std::prev(buffer2.end());
    buffer2.change_predecessor(end, first_pred);
    buffer2.change_successor(end, new_succ1);
    

    // buffer1:        // buffer2:    
    // A -> AAA        // A -> AAA
    // B -> BBB        // B -> BBB
                       // A -> XXX (dupe)
    
    buffer2.change_predecessor(buffer2.begin(), new_pred1);

    // buffer1:        // buffer2:    
    // A -> XXX != AAA // X -> AAA
    // B -> BBB        // B -> BBB
    // X -> AAA        // A -> XXX

    ASSERT_TRUE(lsys->has_rule(first_pred, new_succ1));
    ASSERT_TRUE(lsys->has_rule(new_pred1, first_succ));
    ASSERT_TRUE(has_rule(buffer1, first_pred, new_succ1));
    ASSERT_TRUE(has_rule(buffer1, new_pred1, first_succ));
    ASSERT_TRUE(has_rule(buffer2, first_pred, new_succ1));
    ASSERT_TRUE(has_rule(buffer2, new_pred1, first_succ));
}
