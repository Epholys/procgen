#include <iostream>

#include <gtest/gtest.h>

#include "NewRuleMapBuffer.h"
#include "helper_algorithm.h"

using namespace procgui;

class NewRuleBufferTest :  public ::testing::Test
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
    
    using IntBuffer = NewRuleMapBuffer<IntMap>;
    
    using const_iterator = IntBuffer::const_iterator;
    using successor = IntBuffer::succ;
    using rule = IntBuffer::Rule;

    NewRuleBufferTest() :
        map(std::make_shared<IntMap>(IntMap::rule_map())),
        buffer{map} {
      buffer.add_rule();
      buffer.change_predecessor(std::begin(buffer), pred1);
      buffer.change_successor(std::begin(buffer), succ1);
      buffer.add_rule();
      buffer.change_predecessor(std::next(std::begin(buffer)), pred2);
      buffer.change_successor(std::next(std::begin(buffer)), succ2);
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
                    if (jt->is_duplicate)
                    {
                        return false;
                    }
                }
            }
            return true;
        }

    std::shared_ptr<IntMap> map;
    IntBuffer buffer;
    const char pred1 = 'A';
    const char pred2 = 'B';
    const char pred3 = 'X';
    const char pred4 = 'Y';
    const int succ1 = 0;
    const int succ2 = 1;
    const int succ3 = 2;
    const int succ4 = 3;
};

// --- Test the helper methods ---

TEST_F(NewRuleBufferTest, helper_has_predecessor)
{
    ASSERT_TRUE(has_predecessor(buffer, pred1));
    ASSERT_FALSE(has_predecessor(buffer, pred3));
}

TEST_F(NewRuleBufferTest, helper_has_rule)
{
    ASSERT_TRUE(has_rule(buffer, pred1, succ1));
    ASSERT_FALSE(has_rule(buffer, pred1, succ3));
    ASSERT_FALSE(has_rule(buffer, pred3, succ3));
}

TEST_F(NewRuleBufferTest, helper_has_duplicate)
{
    ASSERT_FALSE(has_duplicate(buffer, buffer.begin()));

    auto first_pred = buffer.begin()->predecessor;
    buffer.add_rule();
    buffer.change_predecessor(std::prev(buffer.end()), first_pred);
    ASSERT_TRUE(has_duplicate(buffer, buffer.begin()));
    ASSERT_TRUE(duplicates_marked(buffer));
}

TEST_F(NewRuleBufferTest, constructor)
{
    // Check if all map' rules are here
    for (auto it = map->get_rules().begin();
         it != map->get_rules().end();
         ++it)
    {
        ASSERT_TRUE(has_rule(buffer, it->first, it->second));
    }
}

TEST_F(NewRuleBufferTest, add_rule)
{
    buffer.add_rule();

    // Check if the last rule is an empty one
    ASSERT_EQ(*std::prev(buffer.end()), rule({}));
}

// -- 'change_predecessor()' is a complexe beast: a lot of these tests were
// created after trial and error --

TEST_F(NewRuleBufferTest, change_predecessor_simple)
{
    auto begin = buffer.begin();
    auto old_pred = begin->predecessor;
    auto old_succ = begin->successor;
    buffer.change_predecessor(begin, pred3);

    ASSERT_TRUE(map->has_rule(pred3, old_succ));
    ASSERT_FALSE(map->has_rule(old_pred, old_succ));
    
    ASSERT_TRUE(has_predecessor(buffer, pred3));
    ASSERT_FALSE(has_predecessor(buffer, old_pred));
}


TEST_F(NewRuleBufferTest, change_predecessor_old_duplicated)
{
    IntMap::rule_map rules = {};
    std::shared_ptr<IntMap> map =
        std::make_shared<IntMap>(rules);
    IntBuffer buffer (map);

    // === SETUP
    buffer.add_rule();
    auto begin = std::begin(buffer);
    buffer.change_predecessor(begin, pred1);
    buffer.change_successor(begin, succ1);

    buffer.add_rule();
    auto next = std::next(std::begin(buffer));
    buffer.change_predecessor(next, pred1);
    buffer.change_successor(next, succ2);

    // BUFFER
    // dup pred succ
    //  f  A    0
    //  t  A    1
    
    // === MODIFICATION
    begin = std::begin(buffer);
    buffer.change_predecessor(begin, pred3);

    // BUFFER
    // dup pred succ
    //  f  X    0
    //  f  A    1   
    
    // === ASSERTIONS

    ASSERT_TRUE(has_rule(buffer, pred3, succ1));
    ASSERT_TRUE(has_rule(buffer, pred1, succ2));
    ASSERT_FALSE(has_rule(buffer, pred1, succ1));
    ASSERT_FALSE(has_duplicate(buffer, std::begin(buffer)));
    ASSERT_FALSE(has_duplicate(buffer, std::next(std::begin(buffer))));
    
    ASSERT_TRUE(map->has_rule(pred3, succ1));
    ASSERT_TRUE(map->has_rule(pred1, succ2));
}


TEST_F(NewRuleBufferTest, change_predecessor_is_duplicated)
{
    auto begin = buffer.begin();
    auto first_pred = begin->predecessor;
    auto first_succ = begin->successor;

    buffer.add_rule();

    auto end = std::prev(buffer.end());
    buffer.change_successor(end, succ3);
    buffer.change_predecessor(end, first_pred);

    ASSERT_TRUE(map->has_rule(first_pred, first_succ));
    
    ASSERT_TRUE(has_rule(buffer, first_pred, succ3));
    ASSERT_TRUE(has_duplicate(buffer, begin));
    ASSERT_TRUE(duplicates_marked(buffer));
}

TEST_F(NewRuleBufferTest, change_predecessor_remove_rule)
{
    auto begin = buffer.begin();
    auto first_pred = begin->predecessor;
    auto first_succ = begin->successor;
    
    buffer.change_predecessor(begin, pred3);

    ASSERT_TRUE(map->has_rule(pred3, first_succ));
    ASSERT_FALSE(map->has_rule(first_pred, first_succ));

    ASSERT_TRUE(has_rule(buffer, pred3, first_succ));
    ASSERT_FALSE(has_rule(buffer, first_pred, first_succ));
}

TEST_F(NewRuleBufferTest, change_predecessor_remove_rule_duplicated)
{
    auto begin = buffer.begin();
    auto first_pred = begin->predecessor;
    auto first_succ = begin->successor;
    buffer.add_rule();
    
    auto end = std::prev(buffer.end());
    buffer.change_predecessor(end, first_pred);
    buffer.change_successor(end, succ3);

    buffer.change_predecessor(end, pred3);
   
    ASSERT_TRUE(map->has_rule(first_pred, first_succ));
    ASSERT_TRUE(map->has_rule(pred3, succ3));
}

TEST_F(NewRuleBufferTest, change_predecessor_double_duplication)
{
    buffer.add_rule();
    auto end = std::prev(buffer.end());
    buffer.change_predecessor(end, pred3);
    buffer.change_successor(end, succ3);

    buffer.add_rule();
    end = std::prev(buffer.end());
    buffer.change_predecessor(end, pred3);
    buffer.change_successor(end, succ4);

    buffer.add_rule();
    end = std::prev(buffer.end());
    buffer.change_predecessor(end, pred3);
    buffer.change_successor(end, pred4+1);

    // buffer:
    // A -> 0
    // B -> 1
    // X -> 2
    // X -> 3 (duplicate) <=to_change
    // X -> 4 (duplicate)

    auto to_change = std::prev(std::prev(buffer.end()));
    buffer.change_predecessor(to_change, pred4);

    // buffer:
    // A -> 0
    // B -> 1
    // X -> 2
    // Y -> 3 
    // X -> 4 (duplicate)

    buffer.change_predecessor(to_change, pred3);

    // buffer (expected):
    // A -> 0
    // B -> 1
    // X -> 2
    // X -> 3 (duplicate)
    // X -> 4 (duplicate)

    ASSERT_TRUE(map->has_rule(pred3, succ3));
    ASSERT_FALSE(map->has_predecessor(pred4));

    ASSERT_TRUE(has_rule(buffer, pred3, succ3));
    ASSERT_FALSE(has_predecessor(buffer, pred4));
    ASSERT_TRUE(duplicates_marked(buffer));
}

TEST_F(NewRuleBufferTest, change_predecessor_duplication_before_after)
{
    auto begin = buffer.begin();
    auto first_pred = begin->predecessor;
    auto first_succ = begin->successor;

    buffer.add_rule();
    auto end = std::prev(buffer.end());
    buffer.change_predecessor(end, pred3);
    buffer.change_successor(end, succ3);

    buffer.add_rule();
    end = std::prev(buffer.end());
    buffer.change_predecessor(end, pred3);
    buffer.change_successor(end, succ4);

    // buffer:
    // A -> 0
    // B -> 1
    // X -> 2 <= to_change
    // X -> 3 (duplicate)
    auto to_change = std::prev(std::prev(buffer.end()));
    buffer.change_predecessor(to_change, first_pred);

    // buffer: (expected)
    // A -> 0
    // B -> 1
    // A -> 2 (duplicate)
    // X -> 3
    ASSERT_TRUE(map->has_rule(first_pred, first_succ));
    ASSERT_TRUE(map->has_rule(pred3, succ4));
    ASSERT_TRUE(has_rule(buffer, first_pred, first_succ));
    ASSERT_TRUE(has_rule(buffer, pred3, succ4));
    ASSERT_TRUE(has_duplicate(buffer, begin));
    ASSERT_TRUE(duplicates_marked(buffer));
}

TEST_F(NewRuleBufferTest, change_predecessor_duplication_go_back)
{
    auto begin = buffer.begin();
    auto first_pred = begin->predecessor;
    auto first_succ = begin->successor;

    buffer.add_rule();
    auto end = std::prev(buffer.end());
    buffer.change_predecessor(end, pred3);
    buffer.change_successor(end, succ3);

    buffer.add_rule();
    end = std::prev(buffer.end());
    buffer.change_predecessor(end, pred3);
    buffer.change_successor(end, succ4);

    // buffer:
    // A -> 0
    // B -> 1
    // X -> 2 <= to_change
    // X -> 3 (duplicate)
    auto to_change = std::prev(std::prev(buffer.end()));
    buffer.change_predecessor(to_change, first_pred);

    // buffer:
    // A -> 0
    // B -> 1
    // A -> 2 (duplicate)
    // X -> 3

    buffer.change_predecessor(to_change, pred3);
    // buffer: (expected)
    // A -> 0
    // B -> 1
    // X -> 2 (duplicate)
    // X -> 3

    ASSERT_TRUE(map->has_rule(first_pred, first_succ));
    ASSERT_TRUE(map->has_rule(pred3, succ4));
    ASSERT_TRUE(has_rule(buffer, first_pred, first_succ));
    ASSERT_TRUE(has_rule(buffer, pred3, succ4));
    ASSERT_TRUE(has_duplicate(buffer, std::prev(buffer.end())));
    ASSERT_TRUE(duplicates_marked(buffer));
}



TEST_F(NewRuleBufferTest, remove_predecessor_simple)
{
    auto begin = buffer.begin();
    auto pred = begin->predecessor;
    buffer.remove_predecessor(begin);

    ASSERT_FALSE(map->has_predecessor(pred));

    ASSERT_FALSE(has_predecessor(buffer, pred));
}

TEST_F(NewRuleBufferTest, remove_predecessor_duplicate)
{
    auto begin = buffer.begin();
    auto first_pred = begin->predecessor;

    buffer.add_rule();

    auto end = std::prev(buffer.end());
    buffer.change_predecessor(end, first_pred);
    buffer.change_successor(end, succ3);
    buffer.remove_predecessor(begin);

    ASSERT_TRUE(map->has_rule(first_pred, succ3));
    ASSERT_TRUE(has_rule(buffer, first_pred, succ3));
}

TEST_F(NewRuleBufferTest, change_successor_simple)
{
    buffer.change_successor(buffer.begin(), succ3);

    auto pred = buffer.begin()->predecessor;

    ASSERT_TRUE(map->has_rule(pred, succ3));
    ASSERT_TRUE(map->has_rule(pred, succ3));
    ASSERT_TRUE(map->has_rule(pred, succ3));
}

TEST_F(NewRuleBufferTest, change_successor_duplicate)
{
    auto begin = buffer.begin();
    auto first_pred = begin->predecessor;
    auto first_succ = begin->successor;

    buffer.add_rule();
    
    auto end = std::prev(buffer.end());
    buffer.change_predecessor(end, first_pred);
    buffer.change_successor(end, succ3);

    ASSERT_FALSE(map->has_rule(first_pred, succ3));

    ASSERT_TRUE(has_rule(buffer, first_pred, first_succ));
    ASSERT_TRUE(has_rule(buffer, first_pred, succ3));
    ASSERT_TRUE(duplicates_marked(buffer));
}

TEST_F(NewRuleBufferTest, erase_simple)
{
    auto size = buffer.size();
    auto pred = buffer.begin()->predecessor;
    buffer.erase(buffer.begin());

    ASSERT_FALSE(map->has_predecessor(pred));
    ASSERT_FALSE(has_predecessor(buffer, pred));
    ASSERT_EQ(size-1, buffer.size());
}

TEST_F(NewRuleBufferTest, erase_duplicate)
{
    auto size = buffer.size();
    auto begin = buffer.begin();
    auto first_pred = begin->predecessor;
    auto first_succ = begin->successor;

    buffer.add_rule();

    auto end = std::prev(buffer.end());
    buffer.change_predecessor(end, first_pred);
    buffer.erase(end);

    ASSERT_TRUE(map->has_rule(first_pred, first_succ));

    ASSERT_TRUE(has_predecessor(buffer, first_pred));
    ASSERT_FALSE(has_duplicate(buffer, buffer.begin()));
    ASSERT_EQ(size, buffer.size());

    ASSERT_EQ(size, buffer.size());
}

TEST_F(NewRuleBufferTest, erase_empty)
{
    auto size = buffer.size();
    buffer.add_rule();
    buffer.erase(std::prev(buffer.end()));
    
    ASSERT_EQ(size, buffer.size());
}

TEST_F(NewRuleBufferTest, erase_replacement)
{
    auto size = buffer.size();
    auto begin  = buffer.begin();
    auto first_pred = begin->predecessor;
    
    buffer.add_rule();
    
    auto end = std::prev(buffer.end());
    buffer.change_predecessor(end, first_pred);
    buffer.change_successor(end, succ3);
    buffer.erase(begin);

    ASSERT_TRUE(map->has_rule(first_pred, succ3));
    
    ASSERT_TRUE(has_rule(buffer, first_pred, succ3));
    ASSERT_EQ(size, buffer.size());
}

// Test if the predecessor is correctly reverted
TEST_F(NewRuleBufferTest, revert_predecessors_new_orig_old_orig_no_dup)
{
    // BUFFER
    // dup pred succ
    //  t  A    0
    //  t  B    1    

    buffer.change_predecessor(std::begin(buffer), pred3);
    // BUFFER
    // dup pred succ
    //  t  X    0
    //  t  B    1
    
    buffer.revert();

    ASSERT_TRUE(has_rule(buffer, pred1, succ1));
    ASSERT_FALSE(has_rule(buffer, pred3, succ1));

    ASSERT_TRUE(map->has_rule(pred1, succ1));
    ASSERT_TRUE(map->has_rule(pred2, succ2));
    
    ASSERT_EQ(2, map->size());
}
TEST_F(NewRuleBufferTest, revert_predecessors_new_dup_old_orig_no_dup)
{
    buffer.add_rule();
    auto end = std::prev(std::end(buffer));
    buffer.change_predecessor(end, pred3);
    buffer.change_successor(end, succ3);
    // BUFFER
    // dup pred succ
    //  t  A    0
    //  t  B    1
    //  t  X    2
    
    buffer.change_predecessor(end, pred1);
    // BUFFER
    // dup pred succ
    //  t  A    0
    //  t  B    1
    //  f  A    2

    buffer.revert();

    ASSERT_TRUE(has_rule(buffer, pred1, succ1));
    ASSERT_TRUE(has_rule(buffer, pred2, succ2));
    ASSERT_TRUE(has_rule(buffer, pred3, succ3));
    ASSERT_FALSE(has_duplicate(buffer, end));
    ASSERT_FALSE(has_rule(buffer, pred1, succ3));

    ASSERT_TRUE(map->has_rule(pred1, succ1));
    ASSERT_TRUE(map->has_rule(pred2, succ2));
    ASSERT_TRUE(map->has_rule(pred3, succ3));
    ASSERT_EQ(3, map->size());
}

TEST_F(NewRuleBufferTest, revert_predecessors_new_orig_old_dup)
{
    buffer.add_rule();
    auto end = std::prev(std::end(buffer));
    buffer.change_predecessor(end, pred1);
    buffer.change_successor(end, succ3);
    // BUFFER
    // dup pred succ
    //  t  A    0
    //  t  B    1
    //  f  A    2
    
    buffer.change_predecessor(end, pred3);
    // BUFFER
    // dup pred succ
    //  t  A    0
    //  t  B    1
    //  t  X    2

    buffer.revert();

    ASSERT_TRUE(has_rule(buffer, pred1, succ1));
    ASSERT_TRUE(has_rule(buffer, pred2, succ2));
    ASSERT_TRUE(has_rule(buffer, pred1, succ3));
    ASSERT_TRUE(has_duplicate(buffer, end));
    ASSERT_FALSE(has_rule(buffer, pred3, succ3));

    ASSERT_TRUE(map->has_rule(pred1, succ1));
    ASSERT_TRUE(map->has_rule(pred2, succ2));
    ASSERT_EQ(2, map->size());
}

TEST_F(NewRuleBufferTest, revert_predecessors_new_dup_old_dup)
{
    buffer.add_rule();
    auto end = std::prev(std::end(buffer));
    buffer.change_predecessor(end, pred1);
    buffer.change_successor(end, succ3);
    // BUFFER
    // dup pred succ
    //  t  A    0
    //  t  B    1
    //  f  A    2
    
    buffer.change_predecessor(end, pred2);
    // BUFFER
    // dup pred succ
    //  t  A    0
    //  t  B    1
    //  f  B    2
    
    buffer.revert();

    ASSERT_TRUE(has_rule(buffer, pred1, succ1));
    ASSERT_TRUE(has_rule(buffer, pred2, succ2));
    ASSERT_TRUE(has_rule(buffer, pred1, succ3));
    ASSERT_TRUE(has_duplicate(buffer, end));
    ASSERT_FALSE(has_rule(buffer, pred2, succ3));

    ASSERT_TRUE(map->has_rule(pred1, succ1));
    ASSERT_TRUE(map->has_rule(pred2, succ2));
    ASSERT_EQ(2, map->size());
}

// diuplicate ?
TEST_F(NewRuleBufferTest, revert_predecessors_new_orig_old_orig_w_dup)
{
    buffer.add_rule();
    auto end = std::prev(std::end(buffer));
    buffer.change_predecessor(end, pred2);
    buffer.change_successor(end, succ3);
    // BUFFER
    // dup pred succ
    //  t  A    0
    //  t  B    1
    //  f  B    2

    buffer.change_predecessor(std::prev(end), pred3);
    // BUFFER
    // dup pred succ
    //  t  A    0
    //  t  X    1
    //  t  B    2
    
    buffer.revert();

    ASSERT_TRUE(has_rule(buffer, pred1, succ1));
    ASSERT_TRUE(has_rule(buffer, pred2, succ2));
    ASSERT_TRUE(has_rule(buffer, pred2, succ3));
    ASSERT_TRUE(has_duplicate(buffer, end));
    ASSERT_FALSE(has_rule(buffer, pred3, succ2));

    ASSERT_TRUE(map->has_rule(pred1, succ1));
    ASSERT_TRUE(map->has_rule(pred2, succ2));
    ASSERT_EQ(2, map->size());
}


TEST_F(NewRuleBufferTest, revert_predecessors_new_dup_old_orig_w_dup)
{
    buffer.add_rule();
    auto end = std::prev(std::end(buffer));
    buffer.change_predecessor(end, pred2);
    buffer.change_successor(end, succ3);
    // BUFFER
    // dup pred succ
    //  t  A    0
    //  t  B    1
    //  f  B    2
    
    buffer.change_predecessor(std::prev(end), pred1);
    // BUFFER
    // dup pred succ
    //  t  A    0
    //  f  A    1    <-
    //  t  B    2

    buffer.revert();

    ASSERT_TRUE(has_rule(buffer, pred1, succ1));
    ASSERT_TRUE(has_rule(buffer, pred2, succ2));
    ASSERT_TRUE(has_rule(buffer, pred2, succ3));
    ASSERT_TRUE(has_duplicate(buffer, end));
    ASSERT_FALSE(has_rule(buffer, pred1, succ2));

    ASSERT_TRUE(map->has_rule(pred1, succ1));
    ASSERT_TRUE(map->has_rule(pred2, succ2));
    ASSERT_EQ(2, map->size());
}


// Test if the succesor is correctly reverted
TEST_F(NewRuleBufferTest, revert_successor)
{
    buffer.change_successor(std::begin(buffer), succ3);
    buffer.revert();

    ASSERT_TRUE(has_rule(buffer, pred1, succ1));
    ASSERT_FALSE(has_rule(buffer, pred1, succ3));
}

// Test if validating a predecessor forbid reverting
TEST_F(NewRuleBufferTest, validate_predecessor)
{
    buffer.change_predecessor(std::begin(buffer), pred3);
    buffer.validate();
    buffer.revert(); //  Should do nothing
    
    ASSERT_TRUE(has_rule(buffer, pred3, succ1));
    ASSERT_FALSE(has_rule(buffer, pred1, succ1));
    ASSERT_FALSE(has_predecessor(buffer, pred1));
}

// Test if validating a successor forbid reverting
TEST_F(NewRuleBufferTest, validate_successor)
{
    buffer.change_successor(std::begin(buffer), succ3);
    buffer.validate();
    buffer.revert();
    
    ASSERT_TRUE(has_rule(buffer, pred1, succ3));
    ASSERT_FALSE(has_rule(buffer, pred1, succ1));
}

TEST_F(NewRuleBufferTest, sync)
{
    IntBuffer buffer2 (map);

    // BUFFER
    // dup pred succ
    //  t  A    0
    //  t  B    1

    buffer.add_rule();
    auto end = std::prev(std::end(buffer));
    buffer.change_predecessor(end, pred1);
    buffer.change_successor(end, succ3);

    // BUFFER
    // dup pred succ
    //  t  A    0
    //  t  B    1
    //  f  A    2
    
    auto begin = std::begin(buffer);
    buffer.change_predecessor(begin, pred3);
    buffer.change_successor(begin, succ4);

    // BUFFER
    // dup pred succ
    //  t  C    3
    //  t  B    1
    //  t  A    2
    
    auto next = std::next(begin);
    buffer.change_predecessor(next, pred4);

    // BUFFER
    // dup pred succ
    //  t  C    3
    //  t  D    1
    //  t  A    2

    // MAP
    // A 2
    // D 1
    // C 3

    ASSERT_TRUE(has_rule(buffer2, pred1, succ3));
    ASSERT_TRUE(has_rule(buffer2, pred3, succ4));
    ASSERT_TRUE(has_rule(buffer2, pred4, succ2));
}
