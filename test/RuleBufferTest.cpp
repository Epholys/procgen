#include <iostream>

#include <gtest/gtest.h>

#include "RuleBuffer.h"
#include "helper_algorithm.h"

using namespace procgui;

class RuleBufferTest :  public ::testing::Test
{
public:
    class IntMap : public RuleMap<int>
    {
    public:
        IntMap(const rule_map& rules)
            : RuleMap<int>{rules}
            {
            }
    };
    
    using IntBuffer = RuleBuffer<IntMap>;
    
    using const_iterator = IntBuffer::const_iterator;
    using successor = IntBuffer::succ;
    using rule = IntBuffer::Rule;

    RuleBufferTest()
        : buffer1 {map}
        , buffer2 {map}
        {
        }

    // Helper methods:

    // Check the existence of the predecessor 'pred' in 'buff'
    bool has_predecessor (const IntBuffer& buff, char pred) const
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

    // Check the existence of the rule "'pred' -> 'succ'" in 'buff'
    bool has_rule (const IntBuffer& buff, char pred, int succ) const
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

    // Check if 'buff' has a duplicate of the rule at 'it'.
    bool has_duplicate(const IntBuffer& buff, const_iterator it) const
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

    // Check if all duplicates in 'buff' are correctly tagged with the 'valid'
    // attribute of the buffer's rules.
    bool duplicates_marked(const IntBuffer& buff) const
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
        
    const char pred1 = 'A';
    const char pred2 = 'B';
    const int succ1 = 0;
    const int succ2 = 1;
    IntMap::rule_map rules = {{pred1, succ1}, {pred2, succ2}};
    std::shared_ptr<IntMap> map = std::make_shared<IntMap>(rules);
    IntBuffer buffer1;
    IntBuffer buffer2;
    const char new_pred1 = 'X';
    const char new_pred2 = 'Y';
    const int new_succ1 = 2;
    const int new_succ2 = 3;
    const int new_succ3 = 4;
};

// --- Test the helper methods ---

TEST_F(RuleBufferTest, helper_has_predecessor)
{
    ASSERT_TRUE(has_predecessor(buffer1, pred1));
    ASSERT_FALSE(has_predecessor(buffer1, new_pred1));
}

TEST_F(RuleBufferTest, helper_has_rule)
{
    ASSERT_TRUE(has_rule(buffer1, pred1, succ1));
    ASSERT_FALSE(has_rule(buffer1, pred1, new_succ1));
    ASSERT_FALSE(has_rule(buffer1, new_pred1, new_succ1));
}

TEST_F(RuleBufferTest, helper_has_duplicate)
{
    ASSERT_FALSE(has_duplicate(buffer1, buffer1.begin()));

    auto first_pred = buffer1.begin()->predecessor;
    buffer1.add_rule();
    buffer1.change_predecessor(std::prev(buffer1.end()), first_pred);
    ASSERT_TRUE(has_duplicate(buffer1, buffer1.begin()));
    ASSERT_TRUE(duplicates_marked(buffer1));
}

// --- In the following tests, 'RuleBuffer:sync()' is tested in the majority
// of tests with 'buffer2' as it can not be tested separately --

TEST_F(RuleBufferTest, constructor)
{
    // Check if all map' rules are here
    for (auto it = map->get_rules().begin();
         it != map->get_rules().end();
         ++it)
    {
        ASSERT_TRUE(has_rule(buffer1, it->first, it->second));
    }
}

TEST_F(RuleBufferTest, add_rule)
{
    buffer1.add_rule();

    // Check if the last rule is an empty one
    ASSERT_EQ(*std::prev(buffer1.end()), rule({}));

    ASSERT_NE(*std::prev(buffer2.end()), rule({}));
}

// -- 'change_predecessor()' is a complexe beast: a lot of these tests were
// created after trial and error --

TEST_F(RuleBufferTest, change_predecessor_simple)
{
    auto begin = buffer1.begin();
    auto old_pred = begin->predecessor;
    auto old_succ = begin->successor;
    buffer1.change_predecessor(begin, new_pred1);

    ASSERT_TRUE(map->has_rule(new_pred1, old_succ));
    ASSERT_FALSE(map->has_rule(old_pred, old_succ));
    
    ASSERT_TRUE(has_predecessor(buffer1, new_pred1));
    ASSERT_FALSE(has_predecessor(buffer1, old_pred));
    
    ASSERT_TRUE(has_predecessor(buffer2, new_pred1));
    ASSERT_FALSE(has_predecessor(buffer2, old_pred));
}

TEST_F(RuleBufferTest, change_predecessor_is_duplicated)
{
    auto begin = buffer1.begin();
    auto first_pred = begin->predecessor;
    auto first_succ = begin->successor;

    buffer1.add_rule();

    auto end = std::prev(buffer1.end());
    buffer1.change_successor(end, new_succ1);
    buffer1.change_predecessor(end, first_pred);

    ASSERT_TRUE(map->has_rule(first_pred, first_succ));
    
    ASSERT_TRUE(has_rule(buffer1, first_pred, new_succ1));
    ASSERT_TRUE(has_duplicate(buffer1, begin));
    ASSERT_TRUE(duplicates_marked(buffer1));

    ASSERT_FALSE(has_rule(buffer2, first_pred, new_succ1));
}

TEST_F(RuleBufferTest, change_predecessor_remove_rule)
{
    auto begin = buffer1.begin();
    auto first_pred = begin->predecessor;
    auto first_succ = begin->successor;
    
    buffer1.change_predecessor(begin, new_pred1);

    ASSERT_TRUE(map->has_rule(new_pred1, first_succ));
    ASSERT_FALSE(map->has_rule(first_pred, first_succ));

    ASSERT_TRUE(has_rule(buffer1, new_pred1, first_succ));
    ASSERT_FALSE(has_rule(buffer1, first_pred, first_succ));

    ASSERT_TRUE(has_rule(buffer2, new_pred1, first_succ));
    ASSERT_FALSE(has_rule(buffer2, first_pred, first_succ));
}

TEST_F(RuleBufferTest, change_predecessor_remove_rule_duplicated)
{
    auto begin = buffer1.begin();
    auto first_pred = begin->predecessor;
    auto first_succ = begin->successor;
    buffer1.add_rule();
    
    auto end = std::prev(buffer1.end());
    buffer1.change_predecessor(end, first_pred);
    buffer1.change_successor(end, new_succ1);

    buffer1.change_predecessor(end, new_pred1);
   
    ASSERT_TRUE(map->has_rule(first_pred, first_succ));
    ASSERT_TRUE(map->has_rule(new_pred1, new_succ1));

    ASSERT_TRUE(has_rule(buffer2, first_pred, first_succ));
    ASSERT_TRUE(has_rule(buffer2, new_pred1, new_succ1));
}

TEST_F(RuleBufferTest, change_predecessor_double_duplication)
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
    // A -> 0
    // B -> 1
    // X -> 2
    // X -> 3 (duplicate) <=to_change
    // X -> 4 (duplicate)

    auto to_change = std::prev(std::prev(buffer1.end()));
    buffer1.change_predecessor(to_change, new_pred2);

    // buffer1:
    // A -> 0
    // B -> 1
    // X -> 2
    // Y -> 3 
    // X -> 4 (duplicate)

    buffer1.change_predecessor(to_change, new_pred1);

    // buffer1 (expected):
    // A -> 0
    // B -> 1
    // X -> 2
    // X -> 3 (duplicate)
    // X -> 4 (duplicate)

    ASSERT_TRUE(map->has_rule(new_pred1, new_succ1));
    ASSERT_FALSE(map->has_predecessor(new_pred2));

    ASSERT_TRUE(has_rule(buffer1, new_pred1, new_succ1));
    ASSERT_FALSE(has_predecessor(buffer1, new_pred2));
    ASSERT_TRUE(duplicates_marked(buffer1));

    ASSERT_TRUE(has_rule(buffer2, new_pred1, new_succ1));
    ASSERT_FALSE(has_predecessor(buffer2, new_pred2));
}

TEST_F(RuleBufferTest, change_predecessor_duplication_before_after)
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
    // A -> 0
    // B -> 1
    // X -> 2 <= to_change
    // X -> 3 (duplicate)
    auto to_change = std::prev(std::prev(buffer1.end()));
    buffer1.change_predecessor(to_change, first_pred);

    // buffer1: (expected)
    // A -> 0
    // B -> 1
    // A -> 2 (duplicate)
    // X -> 3
    ASSERT_TRUE(map->has_rule(first_pred, first_succ));
    ASSERT_TRUE(map->has_rule(new_pred1, new_succ2));
    ASSERT_TRUE(has_rule(buffer1, first_pred, first_succ));
    ASSERT_TRUE(has_rule(buffer1, new_pred1, new_succ2));
    ASSERT_TRUE(has_duplicate(buffer1, begin));
    ASSERT_TRUE(duplicates_marked(buffer1));
    ASSERT_TRUE(has_rule(buffer2, first_pred, first_succ));
    ASSERT_TRUE(has_rule(buffer2, new_pred1, new_succ2));
}

TEST_F(RuleBufferTest, change_predecessor_duplication_go_back)
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
    // A -> 0
    // B -> 1
    // X -> 2 <= to_change
    // X -> 3 (duplicate)
    auto to_change = std::prev(std::prev(buffer1.end()));
    buffer1.change_predecessor(to_change, first_pred);

    // buffer1:
    // A -> 0
    // B -> 1
    // A -> 2 (duplicate)
    // X -> 3

    buffer1.change_predecessor(to_change, new_pred1);
    // buffer1: (expected)
    // A -> 0
    // B -> 1
    // X -> 2 (duplicate)
    // X -> 3

    ASSERT_TRUE(map->has_rule(first_pred, first_succ));
    ASSERT_TRUE(map->has_rule(new_pred1, new_succ2));
    ASSERT_TRUE(has_rule(buffer1, first_pred, first_succ));
    ASSERT_TRUE(has_rule(buffer1, new_pred1, new_succ2));
    ASSERT_TRUE(has_duplicate(buffer1, std::prev(buffer1.end())));
    ASSERT_TRUE(duplicates_marked(buffer1));
    ASSERT_TRUE(has_rule(buffer2, first_pred, first_succ));
    ASSERT_TRUE(has_rule(buffer2, new_pred1, new_succ2));
}



TEST_F(RuleBufferTest, remove_predecessor_simple)
{
    auto begin = buffer1.begin();
    auto pred = begin->predecessor;
    buffer1.remove_predecessor(begin);

    ASSERT_FALSE(map->has_predecessor(pred));

    ASSERT_FALSE(has_predecessor(buffer1, pred));
    
    ASSERT_FALSE(has_predecessor(buffer2, pred));

    // There is a scratch buffer
    ASSERT_TRUE(buffer1.size() == buffer2.size() + 1);
}

TEST_F(RuleBufferTest, remove_predecessor_duplicate)
{
    auto begin = buffer1.begin();
    auto first_pred = begin->predecessor;

    buffer1.add_rule();

    auto end = std::prev(buffer1.end());
    buffer1.change_predecessor(end, first_pred);
    buffer1.change_successor(end, new_succ1);
    buffer1.remove_predecessor(begin);

    ASSERT_TRUE(map->has_rule(first_pred, new_succ1));
    ASSERT_TRUE(has_rule(buffer1, first_pred, new_succ1));
    ASSERT_TRUE(has_rule(buffer2, first_pred, new_succ1));
}

TEST_F(RuleBufferTest, change_successor_simple)
{
    buffer1.change_successor(buffer1.begin(), new_succ1);

    auto pred = buffer1.begin()->predecessor;

    ASSERT_TRUE(map->has_rule(pred, new_succ1));
    ASSERT_TRUE(map->has_rule(pred, new_succ1));
    ASSERT_TRUE(map->has_rule(pred, new_succ1));
}

TEST_F(RuleBufferTest, change_successor_duplicate)
{
    auto begin = buffer1.begin();
    auto first_pred = begin->predecessor;
    auto first_succ = begin->successor;

    buffer1.add_rule();
    
    auto end = std::prev(buffer1.end());
    buffer1.change_predecessor(end, first_pred);
    buffer1.change_successor(end, new_succ1);

    ASSERT_FALSE(map->has_rule(first_pred, new_succ1));

    ASSERT_TRUE(has_rule(buffer1, first_pred, first_succ));
    ASSERT_TRUE(has_rule(buffer1, first_pred, new_succ1));
    ASSERT_TRUE(duplicates_marked(buffer1));

    ASSERT_FALSE(has_rule(buffer2, first_pred, new_succ1));
}

TEST_F(RuleBufferTest, erase_simple)
{
    auto size = buffer1.size();
    auto pred = buffer1.begin()->predecessor;
    buffer1.erase(buffer1.begin());

    ASSERT_FALSE(map->has_predecessor(pred));
    ASSERT_FALSE(has_predecessor(buffer1, pred));
    ASSERT_EQ(size-1, buffer1.size());
    ASSERT_FALSE(has_predecessor(buffer2, pred));
    ASSERT_EQ(size-1, buffer2.size());
}

TEST_F(RuleBufferTest, erase_duplicate)
{
    auto size = buffer1.size();
    auto begin = buffer1.begin();
    auto first_pred = begin->predecessor;
    auto first_succ = begin->successor;

    buffer1.add_rule();

    auto end = std::prev(buffer1.end());
    buffer1.change_predecessor(end, first_pred);
    buffer1.erase(end);

    ASSERT_TRUE(map->has_rule(first_pred, first_succ));

    ASSERT_TRUE(has_predecessor(buffer1, first_pred));
    ASSERT_FALSE(has_duplicate(buffer1, buffer1.begin()));
    ASSERT_EQ(size, buffer1.size());

    ASSERT_TRUE(has_predecessor(buffer2, first_pred));
    ASSERT_FALSE(has_duplicate(buffer2, buffer2.begin()));
    ASSERT_EQ(size, buffer1.size());
}

TEST_F(RuleBufferTest, erase_empty)
{
    auto size = buffer1.size();
    buffer1.add_rule();
    buffer1.erase(std::prev(buffer1.end()));
    
    ASSERT_EQ(size, buffer1.size());
}

TEST_F(RuleBufferTest, erase_replacement)
{
    auto size = buffer1.size();
    auto begin  = buffer1.begin();
    auto first_pred = begin->predecessor;
    
    buffer1.add_rule();
    
    auto end = std::prev(buffer1.end());
    buffer1.change_predecessor(end, first_pred);
    buffer1.change_successor(end, new_succ1);
    buffer1.erase(begin);

    ASSERT_TRUE(map->has_rule(first_pred, new_succ1));
    
    ASSERT_TRUE(has_rule(buffer1, first_pred, new_succ1));
    ASSERT_EQ(size, buffer1.size());

    ASSERT_TRUE(has_rule(buffer2, first_pred, new_succ1));
    ASSERT_EQ(size, buffer2.size());
}

TEST_F(RuleBufferTest, advanced_sync_and_layout1)
{
    // Example:
    // buffer1:  // buffer2:    
    // A -> 0    // A -> 0
    // B -> 1    // B -> 1
    
    buffer1.add_rule();
    auto end = std::prev(buffer1.end());
    buffer1.change_predecessor(end, new_pred1);
    buffer1.change_successor(end, new_succ1);

    buffer1.add_rule();
    end = std::prev(buffer1.end());
    buffer1.change_predecessor(end, new_pred1);
    buffer1.change_successor(end, new_succ2);
    
    // buffer1:      // buffer2:    
    // A -> 0        // A -> 0
    // B -> 1        // B -> 1
    // X -> 2        // X -> 2
    // X -> 3 (dupe) // 

    end = std::prev(buffer2.end());
    buffer2.erase(end);

    // buffer1:      // buffer2:    
    // A -> 0        // A -> 0
    // B -> 1        // B -> 1
    // X -> 3        // X -> 3
  
    ASSERT_TRUE(map->has_rule(new_pred1, new_succ2));
    ASSERT_TRUE(has_predecessor(buffer1, new_pred1));
    ASSERT_FALSE(has_duplicate(buffer1, std::prev(buffer1.end())));
    ASSERT_TRUE(has_predecessor(buffer2, new_pred1));
    ASSERT_FALSE(has_duplicate(buffer2, std::prev(buffer2.end())));
}

// Synchronize in priority the window in which we remove a rule.
TEST_F(RuleBufferTest, advanced_sync_and_layout2)
{
    // Example:
    // buffer1:  // buffer2:    
    // A -> 0    // A -> 0
    // B -> 1    // B -> 1
    
    auto first_pred = buffer1.begin()->predecessor;
    auto first_succ = buffer1.begin()->successor;
    buffer2.add_rule();
    auto end = std::prev(buffer2.end());
    buffer2.change_predecessor(end, first_pred);
    buffer2.change_successor(end, new_succ1);
    

    // buffer1:      // buffer2:    
    // A -> 0        // A -> 0
    // B -> 1        // B -> 1
                     // A -> 2 (dupe)
    
    buffer2.change_predecessor(buffer2.begin(), new_pred1);

    // buffer1:      // buffer2:    
    // A -> 2 != 0   // X -> 0
    // B -> 1        // B -> 1
    // X -> 0        // A -> 2

    ASSERT_TRUE(map->has_rule(first_pred, new_succ1));
    ASSERT_TRUE(map->has_rule(new_pred1, first_succ));
    ASSERT_TRUE(has_rule(buffer1, first_pred, new_succ1));
    ASSERT_TRUE(has_rule(buffer1, new_pred1, first_succ));
    ASSERT_TRUE(has_rule(buffer2, first_pred, new_succ1));
    ASSERT_TRUE(has_rule(buffer2, new_pred1, first_succ));
}
