#include <iostream>

#include <gtest/gtest.h>

#include "RuleMapBuffer.h"
#include "helper_algorithm.h"

using namespace procgui;


using IntMap = RuleMap<int>;
using IntMapPtr = std::shared_ptr<IntMap>;
using IntBuffer = RuleMapBuffer<IntMap>;
using IntBufferPtr = std::shared_ptr<IntBuffer>;
class IntBufferObs
{
public:
    explicit IntBufferObs(IntBufferPtr ptr)
        : obs_(ptr)
        {
            obs_.add_callback([this](){notified_ = true;});
        }

    explicit operator bool() const
        {
            return notified_;
        }
    
private:
    Observer<IntBuffer> obs_ {nullptr};
    bool notified_ {false};
};

class RuleBufferTest :  public ::testing::Test
{
public:
    using const_iterator = IntBuffer::const_iterator;
    using successor = IntBuffer::succ;
    using rule = IntBuffer::Rule;

    RuleBufferTest()
        : map(std::make_shared<IntMap>(IntMap::rule_map()))
        , buffer(std::make_shared<IntBuffer>(map))
        {
            buffer->add_rule();
            buffer->change_predecessor(buffer->begin(), pred1);
            buffer->change_successor(buffer->begin(), succ1);
            buffer->add_rule();
            buffer->change_predecessor(std::next(buffer->begin()), pred2);
            buffer->change_successor(std::next(buffer->begin()), succ2);

            // Pointer because only 1 ctor is not deleted and obs must be
            // initializated after the previous modifications
            obs = new IntBufferObs(buffer);
        }

    ~RuleBufferTest()
        {
            delete obs;
        }
    
    // Helper methods:

    // Check the existence of the predecessor 'pred' in 'buff'
    bool has_predecessor (IntBufferPtr buff, char pred) const
        {
            for (auto rule : *buff)
            {
                if (rule.predecessor == pred)
                {
                    return true;
                }
            }
            return false;
        }

    // Check the existence of the rule "'pred' -> 'succ'" in 'buff'
    bool has_rule (IntBufferPtr buff, char pred, int succ) const
        {
            for (auto rule : *buff)
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
    bool has_duplicate(IntBufferPtr buff, const_iterator it) const
        {
            for (auto jt = buff->begin(); jt != buff->end(); ++jt)
            {
                if (it != jt &&
                    it->predecessor == jt->predecessor)
                {
                    return true;
                }
            }
            return false;
        }

    // Check if all duplicates in 'buff' are correctly tagged with the 'active'
    // attribute of the buffer's rules.
    bool duplicates_marked(IntBufferPtr buff) const
        {
            for(auto it = buff->begin(); it != buff->end(); ++it)
            {
                auto jt = it;
                while ((jt = find_duplicate(it, jt, buff->end())) != buff->end())
                {
                    if (jt->is_active)
                    {
                        return false;
                    }
                }
            }
            return true;
        }

    IntMapPtr map;
    IntBufferPtr buffer;
    IntBufferObs* obs;
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

TEST_F(RuleBufferTest, helper_has_predecessor)
{
    ASSERT_TRUE(has_predecessor(buffer, pred1));
    ASSERT_FALSE(has_predecessor(buffer, pred3));
}

TEST_F(RuleBufferTest, helper_has_rule)
{
    ASSERT_TRUE(has_rule(buffer, pred1, succ1));
    ASSERT_FALSE(has_rule(buffer, pred1, succ3));
    ASSERT_FALSE(has_rule(buffer, pred3, succ3));
}

TEST_F(RuleBufferTest, helper_has_duplicate)
{
    ASSERT_FALSE(has_duplicate(buffer, buffer->begin()));

    auto first_pred = buffer->begin()->predecessor;
    buffer->add_rule();
    buffer->change_predecessor(std::prev(buffer->end()), first_pred);
    ASSERT_TRUE(has_duplicate(buffer, buffer->begin()));
    ASSERT_TRUE(duplicates_marked(buffer));
}

TEST_F(RuleBufferTest, constructor)
{
    // Check if all map' rules are here
    for (auto it = map->get_rules().begin();
         it != map->get_rules().end();
         ++it)
    {
        ASSERT_TRUE(has_rule(buffer, it->first, it->second));
    }
}

TEST_F(RuleBufferTest, add_rule)
{
    buffer->add_rule();

    // Check if the last rule is an empty one
    ASSERT_EQ(*std::prev(buffer->end()), rule({}));

    ASSERT_FALSE(*obs);
}

// 'change_predecessor()' is a complexe beast: a lot of these tests were
// created after trial and error
// This is the first part, the old trial an error way.
TEST_F(RuleBufferTest, change_predecessor_simple)
{
    auto begin = buffer->begin();
    auto old_pred = begin->predecessor;
    auto old_succ = begin->successor;
    buffer->change_predecessor(begin, pred3);

    ASSERT_TRUE(map->has_rule(pred3, old_succ));
    ASSERT_FALSE(map->has_rule(old_pred, old_succ));
    
    ASSERT_TRUE(has_predecessor(buffer, pred3));
    ASSERT_FALSE(has_predecessor(buffer, old_pred));

    ASSERT_TRUE(*obs);
}


TEST_F(RuleBufferTest, change_predecessor_old_duplicated)
{
    IntMap::rule_map rules = {};
    std::shared_ptr<IntMap> map = std::make_shared<IntMap>(rules);
    IntBufferPtr buffer = std::make_shared<IntBuffer>(map);
    IntBufferObs obs (buffer);
    
    // === SETUP
    buffer->add_rule();
    auto begin = buffer->begin();
    buffer->change_predecessor(begin, pred1);
    buffer->change_successor(begin, succ1);

    buffer->add_rule();
    auto next = std::next(buffer->begin());
    buffer->change_predecessor(next, pred1);
    buffer->change_successor(next, succ2);

    // BUFFER
    // dup pred succ
    //  f  A    0
    //  t  A    1
    
    // === MODIFICATION
    begin = buffer->begin();
    buffer->change_predecessor(begin, pred3);

    // BUFFER
    // dup pred succ
    //  f  X    0
    //  f  A    1   
    
    // === ASSERTIONS

    ASSERT_TRUE(has_rule(buffer, pred3, succ1));
    ASSERT_TRUE(has_rule(buffer, pred1, succ2));
    ASSERT_FALSE(has_rule(buffer, pred1, succ1));
    ASSERT_FALSE(has_duplicate(buffer, buffer->begin()));
    ASSERT_FALSE(has_duplicate(buffer, std::next(buffer->begin())));
    
    ASSERT_TRUE(map->has_rule(pred3, succ1));
    ASSERT_TRUE(map->has_rule(pred1, succ2));

    ASSERT_TRUE(obs);
}


TEST_F(RuleBufferTest, change_predecessor_is_duplicated)
{
    auto begin = buffer->begin();
    auto first_pred = begin->predecessor;
    auto first_succ = begin->successor;

    buffer->add_rule();

    auto end = std::prev(buffer->end());
    buffer->change_successor(end, succ3);
    buffer->change_predecessor(end, first_pred);

    ASSERT_TRUE(map->has_rule(first_pred, first_succ));
    
    ASSERT_TRUE(has_rule(buffer, first_pred, succ3));
    ASSERT_TRUE(has_duplicate(buffer, begin));
    ASSERT_TRUE(duplicates_marked(buffer));

    ASSERT_TRUE(*obs);
}

TEST_F(RuleBufferTest, change_predecessor_remove_rule)
{
    auto begin = buffer->begin();
    auto first_pred = begin->predecessor;
    auto first_succ = begin->successor;
    
    buffer->change_predecessor(begin, pred3);

    ASSERT_TRUE(map->has_rule(pred3, first_succ));
    ASSERT_FALSE(map->has_rule(first_pred, first_succ));

    ASSERT_TRUE(has_rule(buffer, pred3, first_succ));
    ASSERT_FALSE(has_rule(buffer, first_pred, first_succ));

    ASSERT_TRUE(*obs);
}

TEST_F(RuleBufferTest, change_predecessor_remove_rule_duplicated)
{
    auto begin = buffer->begin();
    auto first_pred = begin->predecessor;
    auto first_succ = begin->successor;
    buffer->add_rule();
    
    auto end = std::prev(buffer->end());
    buffer->change_predecessor(end, first_pred);
    buffer->change_successor(end, succ3);

    buffer->change_predecessor(end, pred3);
   
    ASSERT_TRUE(map->has_rule(first_pred, first_succ));
    ASSERT_TRUE(map->has_rule(pred3, succ3));

    ASSERT_TRUE(*obs);
}

TEST_F(RuleBufferTest, change_predecessor_double_duplication)
{
    buffer->add_rule();
    auto end = std::prev(buffer->end());
    buffer->change_predecessor(end, pred3);
    buffer->change_successor(end, succ3);

    buffer->add_rule();
    end = std::prev(buffer->end());
    buffer->change_predecessor(end, pred3);
    buffer->change_successor(end, succ4);

    buffer->add_rule();
    end = std::prev(buffer->end());
    buffer->change_predecessor(end, pred3);
    buffer->change_successor(end, pred4+1);

    // buffer:
    // A -> 0
    // B -> 1
    // X -> 2
    // X -> 3 (duplicate) <=to_change
    // X -> 4 (duplicate)

    auto to_change = std::prev(std::prev(buffer->end()));
    buffer->change_predecessor(to_change, pred4);

    // buffer:
    // A -> 0
    // B -> 1
    // X -> 2
    // Y -> 3 
    // X -> 4 (duplicate)

    buffer->change_predecessor(to_change, pred3);

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

    ASSERT_TRUE(*obs);
}

TEST_F(RuleBufferTest, change_predecessor_duplication_before_after)
{
    auto begin = buffer->begin();
    auto first_pred = begin->predecessor;
    auto first_succ = begin->successor;

    buffer->add_rule();
    auto end = std::prev(buffer->end());
    buffer->change_predecessor(end, pred3);
    buffer->change_successor(end, succ3);

    buffer->add_rule();
    end = std::prev(buffer->end());
    buffer->change_predecessor(end, pred3);
    buffer->change_successor(end, succ4);

    // buffer:
    // A -> 0
    // B -> 1
    // X -> 2 <= to_change
    // X -> 3 (duplicate)
    auto to_change = std::prev(std::prev(buffer->end()));
    buffer->change_predecessor(to_change, first_pred);

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

    ASSERT_TRUE(*obs);
}

TEST_F(RuleBufferTest, change_predecessor_duplication_go_back)
{
    // BUFFER
    // dup pred succ
    //  t  A    0
    //  t  B    1
    
    buffer->add_rule();
    auto end = std::prev(buffer->end());
    buffer->change_predecessor(end, pred3);
    buffer->change_successor(end, succ3);
    
     // BUFFER
    // dup pred succ
    //  t  A    0
    //  t  B    1
    //  t  X    2
    
    buffer->add_rule();
    end = std::prev(buffer->end());
    buffer->change_predecessor(end, pred3);
    buffer->change_successor(end, succ4);

    // BUFFER
    // dup pred succ
    //  t  A    0
    //  t  B    1
    //  t  X    2    <- to_change
    //  f  X    3

    auto to_change = std::prev(std::prev(buffer->end()));
    buffer->change_predecessor(to_change, pred1);

    // BUFFER
    // dup pred succ
    //  t  A    0
    //  t  B    1
    //  f  A    2    <- to_change
    //  t  X    3

    buffer->change_predecessor(to_change, pred3);
    // BUFFER
    // dup pred succ
    //  t  A    0
    //  t  B    1
    //  f  X    2
    //  t  X    3


    ASSERT_TRUE(has_rule(buffer, pred1, succ1));
    ASSERT_TRUE(has_rule(buffer, pred2, succ2));
    ASSERT_TRUE(has_rule(buffer, pred3, succ4));
    
    ASSERT_TRUE(has_duplicate(buffer, std::prev(buffer->end())));

    ASSERT_TRUE(duplicates_marked(buffer));
    
    ASSERT_TRUE(map->has_rule(pred1, succ1));
    ASSERT_TRUE(map->has_rule(pred2, succ2));
    ASSERT_TRUE(map->has_rule(pred3, succ4));

    ASSERT_TRUE(*obs);
}

// This the second part where each code path is explored

TEST_F(RuleBufferTest, change2_predecessors_new_orig_old_orig_no_dup)
{
    // BUFFER
    // dup pred succ
    //  t  A    0
    //  t  B    1    

    buffer->change_predecessor(buffer->begin(), pred3);
    // BUFFER
    // dup pred succ
    //  t  X    0
    //  t  B    1
    
    ASSERT_TRUE(has_rule(buffer, pred2, succ2));
    ASSERT_TRUE(has_rule(buffer, pred3, succ1));
    ASSERT_FALSE(has_rule(buffer, pred1, succ1));

    ASSERT_TRUE(map->has_rule(pred2, succ2));
    ASSERT_TRUE(map->has_rule(pred3, succ1));
    
    ASSERT_EQ(2, map->size());

    ASSERT_TRUE(*obs);
}
TEST_F(RuleBufferTest, change2_predecessors_new_dup_old_orig_no_dup)
{
    buffer->add_rule();
    auto end = std::prev(buffer->end());
    buffer->change_predecessor(end, pred3);
    buffer->change_successor(end, succ3);
    // BUFFER
    // dup pred succ
    //  t  A    0
    //  t  B    1
    //  t  X    2
    
    buffer->change_predecessor(end, pred1);
    // BUFFER
    // dup pred succ
    //  t  A    0
    //  t  B    1
    //  f  A    2

    ASSERT_TRUE(has_rule(buffer, pred1, succ1));
    ASSERT_TRUE(has_rule(buffer, pred2, succ2));
    ASSERT_FALSE(has_rule(buffer, pred3, succ3));

    ASSERT_TRUE(has_duplicate(buffer, buffer->begin()));

    ASSERT_TRUE(map->has_rule(pred1, succ1));
    ASSERT_TRUE(map->has_rule(pred2, succ2));

    ASSERT_EQ(2, map->size());

    ASSERT_TRUE(*obs);
}

TEST_F(RuleBufferTest, change2_predecessors_new_orig_old_dup)
{
    buffer->add_rule();
    auto end = std::prev(buffer->end());
    buffer->change_predecessor(end, pred1);
    buffer->change_successor(end, succ3);
    // BUFFER
    // dup pred succ
    //  t  A    0
    //  t  B    1
    //  f  A    2
    
    buffer->change_predecessor(end, pred3);
    // BUFFER
    // dup pred succ
    //  t  A    0
    //  t  B    1
    //  t  X    2

    ASSERT_TRUE(has_rule(buffer, pred1, succ1));
    ASSERT_TRUE(has_rule(buffer, pred2, succ2));
    ASSERT_TRUE(has_rule(buffer, pred3, succ3));
    ASSERT_FALSE(has_rule(buffer, pred1, succ3));

    ASSERT_TRUE(map->has_rule(pred1, succ1));
    ASSERT_TRUE(map->has_rule(pred2, succ2));
    ASSERT_TRUE(map->has_rule(pred2, succ2));
        
    ASSERT_EQ(3, map->size());

    ASSERT_TRUE(*obs);
}

TEST_F(RuleBufferTest, change2_predecessors_new_dup_old_dup)
{
    buffer->add_rule();
    auto end = std::prev(buffer->end());
    buffer->change_predecessor(end, pred1);
    buffer->change_successor(end, succ3);
    // BUFFER
    // dup pred succ
    //  t  A    0
    //  t  B    1
    //  f  A    2
    
    buffer->change_predecessor(end, pred2);
    // BUFFER
    // dup pred succ
    //  t  A    0
    //  t  B    1
    //  f  B    2    

    ASSERT_TRUE(has_rule(buffer, pred1, succ1));
    ASSERT_TRUE(has_rule(buffer, pred2, succ2));
    ASSERT_TRUE(has_rule(buffer, pred2, succ3));
    ASSERT_TRUE(has_duplicate(buffer, end));
    ASSERT_FALSE(has_rule(buffer, pred1, succ3));

    ASSERT_TRUE(map->has_rule(pred1, succ1));
    ASSERT_TRUE(map->has_rule(pred2, succ2));
    ASSERT_EQ(2, map->size());

    ASSERT_TRUE(*obs);
}

// diuplicate ?
TEST_F(RuleBufferTest, change2_predecessors_new_orig_old_orig_w_dup)
{

    buffer->add_rule();
    auto end = std::prev(buffer->end());
    buffer->change_predecessor(end, pred2);
    buffer->change_successor(end, succ3);
    // BUFFER
    // dup pred succ
    //  t  A    0
    //  t  B    1
    //  f  B    2


    buffer->change_predecessor(std::prev(end), pred3);
    // BUFFER
    // dup pred succ
    //  t  A    0
    //  t  X    1
    //  t  B    2

    
    ASSERT_TRUE(has_rule(buffer, pred1, succ1));
    ASSERT_TRUE(has_rule(buffer, pred3, succ2));
    ASSERT_TRUE(has_rule(buffer, pred2, succ3));

    ASSERT_FALSE(has_rule(buffer, pred2, succ2));

    ASSERT_TRUE(map->has_rule(pred1, succ1));
    ASSERT_TRUE(map->has_rule(pred3, succ2));
    ASSERT_TRUE(map->has_rule(pred2, succ3));
    ASSERT_EQ(3, map->size());

    ASSERT_TRUE(*obs);
}


TEST_F(RuleBufferTest, change2_predecessors_new_dup_old_orig_w_dup)
{
    buffer->add_rule();
    auto end = std::prev(buffer->end());
    buffer->change_predecessor(end, pred2);
    buffer->change_successor(end, succ3);
    // BUFFER
    // dup pred succ
    //  t  A    0
    //  t  B    1
    //  f  B    2
    
    buffer->change_predecessor(std::prev(end), pred1);
    // BUFFER
    // dup pred succ
    //  t  A    0
    //  f  A    1    <-
    //  t  B    2

    ASSERT_TRUE(has_rule(buffer, pred1, succ1));
    ASSERT_TRUE(has_rule(buffer, pred1, succ2));
    ASSERT_TRUE(has_rule(buffer, pred2, succ3));
    ASSERT_TRUE(has_duplicate(buffer, std::prev(end)));
    ASSERT_FALSE(has_rule(buffer, pred2, succ2));

    ASSERT_TRUE(map->has_rule(pred1, succ1));
    ASSERT_TRUE(map->has_rule(pred2, succ3));
    ASSERT_EQ(2, map->size());

    ASSERT_TRUE(*obs);
}




TEST_F(RuleBufferTest, remove_predecessor_simple)
{
    auto begin = buffer->begin();
    auto pred = begin->predecessor;
    buffer->remove_predecessor(begin);

    ASSERT_FALSE(map->has_predecessor(pred));
    ASSERT_FALSE(has_predecessor(buffer, pred));
    ASSERT_EQ(1, map->size());

    ASSERT_TRUE(*obs);
}

TEST_F(RuleBufferTest, remove_predecessor_to_duplicate)
{
    auto begin = buffer->begin();
    auto first_pred = begin->predecessor;

    // BUFFER
    // dup pred succ
    //  t  A    0
    //  t  B    1
    
    buffer->add_rule();    
    auto end = std::prev(buffer->end());
    buffer->change_predecessor(end, first_pred);
    buffer->change_successor(end, succ3);

    // BUFFER
    // dup pred succ
    //  t  A    0
    //  t  B    1
    //  f  A    2

    buffer->remove_predecessor(begin);
    // BUFFER
    // dup pred succ
    //  t  B    1
    //  t  A    2

    
    ASSERT_TRUE(map->has_rule(first_pred, succ3));
    ASSERT_TRUE(has_rule(buffer, first_pred, succ3));

    ASSERT_TRUE(*obs);
}


TEST_F(RuleBufferTest, remove_predecessor_from_duplicate)
{
    auto begin = buffer->begin();
    auto first_pred = begin->predecessor;

    // BUFFER
    // dup pred succ
    //  t  A    0
    //  t  B    1
    
    buffer->add_rule();    
    auto end = std::prev(buffer->end());
    buffer->change_predecessor(end, first_pred);
    buffer->change_successor(end, succ3);

    // BUFFER
    // dup pred succ
    //  t  A    0
    //  t  B    1
    //  f  A    2

    buffer->remove_predecessor(std::prev(buffer->end()));

    // BUFFER
    // dup pred succ
    //  t  A    0
    //  t  B    1
    
    ASSERT_TRUE(has_rule(buffer, first_pred, succ1));
    ASSERT_FALSE(has_rule(buffer, first_pred, succ3));

    ASSERT_TRUE(map->has_rule(first_pred, succ1));

    ASSERT_TRUE(*obs);
}


TEST_F(RuleBufferTest, change_successor_simple)
{
    buffer->change_successor(buffer->begin(), succ3);

    auto pred = buffer->begin()->predecessor;

    ASSERT_TRUE(map->has_rule(pred, succ3));
    ASSERT_TRUE(map->has_rule(pred, succ3));
    ASSERT_TRUE(map->has_rule(pred, succ3));

    ASSERT_TRUE(*obs);
}

TEST_F(RuleBufferTest, change_successor_duplicate)
{
    auto begin = buffer->begin();
    auto first_pred = begin->predecessor;
    auto first_succ = begin->successor;

    buffer->add_rule();
    
    auto end = std::prev(buffer->end());
    buffer->change_predecessor(end, first_pred);
    buffer->change_successor(end, succ3);

    ASSERT_FALSE(map->has_rule(first_pred, succ3));

    ASSERT_TRUE(has_rule(buffer, first_pred, first_succ));
    ASSERT_TRUE(has_rule(buffer, first_pred, succ3));
    ASSERT_TRUE(duplicates_marked(buffer));

    ASSERT_TRUE(*obs);
}

TEST_F(RuleBufferTest, erase_simple)
{
    auto size = buffer->size();
    auto pred = buffer->begin()->predecessor;
    buffer->erase(buffer->begin());

    ASSERT_FALSE(map->has_predecessor(pred));
    ASSERT_FALSE(has_predecessor(buffer, pred));
    ASSERT_EQ(size-1, buffer->size());

    ASSERT_TRUE(*obs);
}

TEST_F(RuleBufferTest, erase_to_duplicate)
{
    auto size = buffer->size();
    auto begin = buffer->begin();
    auto first_pred = begin->predecessor;
    auto first_succ = begin->successor;
    // BUFFER
    // dup pred succ
    //  t  A    0
    //  t  B    1
    
    buffer->add_rule();
    auto end = std::prev(buffer->end());
    buffer->change_predecessor(end, first_pred);

    // BUFFER
    // dup pred succ
    //  t  A    0
    //  t  B    1
    //  f  A    x

    buffer->erase(end);
    
    // BUFFER
    // dup pred succ
    //  t  A    0
    //  t  B    1


    ASSERT_TRUE(map->has_rule(first_pred, first_succ));

    ASSERT_TRUE(has_predecessor(buffer, first_pred));
    ASSERT_FALSE(has_duplicate(buffer, buffer->begin()));
    ASSERT_EQ(size, buffer->size());

    ASSERT_EQ(size, buffer->size());

    ASSERT_TRUE(*obs);
}


TEST_F(RuleBufferTest, erase_from_duplicate)
{
    auto size = buffer->size();
    auto begin = buffer->begin();
    auto first_pred = begin->predecessor;
    auto first_succ = begin->successor;
    // BUFFER
    // dup pred succ
    //  t  A    0
    //  t  B    1
    
    buffer->add_rule();
    auto end = std::prev(buffer->end());
    buffer->change_predecessor(end, first_pred);
    buffer->change_successor(end, succ3);
    
    // BUFFER
    // dup pred succ
    //  t  A    0
    //  t  B    1
    //  f  A    2

    std::cout << "BUFF\n";
    for (const auto& rule : *buffer)
    {
        std::cout << rule.is_active << " " << rule.predecessor << " " << rule.successor << '\n';
    }
    std::cout << "\n";
    
    buffer->erase(buffer->begin());
    
    // BUFFER
    // dup pred succ
    //  t  B    1
    //  t  A    2

    std::cout << "BUFF\n";
    for (const auto& rule : *buffer)
    {
        std::cout << rule.is_active << " " << rule.predecessor << " " << rule.successor << '\n';
    }
    std::cout << "\n";
    
    
    ASSERT_TRUE(has_rule(buffer, first_pred, succ3));
    ASSERT_TRUE(has_rule(buffer, pred2, succ2));    
    ASSERT_FALSE(has_rule(buffer, pred1, succ1));    

    ASSERT_TRUE(std::prev(buffer->end())->is_active);
    
    ASSERT_TRUE(map->has_rule(first_pred, succ3));
    ASSERT_TRUE(map->has_rule(pred2, succ2));

    ASSERT_TRUE(*obs);
}


TEST_F(RuleBufferTest, erase_empty)
{
    auto size = buffer->size();
    buffer->add_rule();
    buffer->erase(std::prev(buffer->end()));
    
    ASSERT_EQ(size, buffer->size());

    ASSERT_FALSE(*obs);
}

TEST_F(RuleBufferTest, erase_replacement)
{
    auto size = buffer->size();
    auto begin  = buffer->begin();
    auto first_pred = begin->predecessor;
    
    buffer->add_rule();
    
    auto end = std::prev(buffer->end());
    buffer->change_predecessor(end, first_pred);
    buffer->change_successor(end, succ3);
    buffer->erase(begin);

    ASSERT_TRUE(map->has_rule(first_pred, succ3));
    
    ASSERT_TRUE(has_rule(buffer, first_pred, succ3));
    ASSERT_EQ(size, buffer->size());

    ASSERT_TRUE(*obs);
}

// Test if the succesor is correctly reverted
TEST_F(RuleBufferTest, revert_successor)
{
    buffer->change_successor(buffer->begin(), succ3);
    buffer->revert();

    ASSERT_TRUE(has_rule(buffer, pred1, succ1));
    ASSERT_FALSE(has_rule(buffer, pred1, succ3));
}

// Test if validating a predecessor forbid reverting
TEST_F(RuleBufferTest, validate_predecessor)
{
    buffer->change_predecessor(buffer->begin(), pred3);
    buffer->validate();
    buffer->revert(); //  Should do nothing
    
    ASSERT_TRUE(has_rule(buffer, pred3, succ1));
    ASSERT_FALSE(has_rule(buffer, pred1, succ1));
    ASSERT_FALSE(has_predecessor(buffer, pred1));
}

// Test if validating a successor forbid reverting
TEST_F(RuleBufferTest, validate_successor)
{
    buffer->change_successor(buffer->begin(), succ3);
    buffer->validate();
    buffer->revert();
    
    ASSERT_TRUE(has_rule(buffer, pred1, succ3));
    ASSERT_FALSE(has_rule(buffer, pred1, succ1));
}


// Revert

TEST_F(RuleBufferTest, revert_add_rule)
{
    // BUFFER
    // dup pred succ
    //  t  A    0
    //  t  B    1

    buffer->add_rule();
    // BUFFER
    // dup pred succ
    //  t  A    0
    //  t  B    1
    //  t  \0   \0

    buffer->revert();

    ASSERT_TRUE(has_rule(buffer, pred1, succ1));
    ASSERT_TRUE(has_rule(buffer, pred2, succ2));
    ASSERT_EQ(2, buffer->size());

    ASSERT_TRUE(map->has_rule(pred1, succ1));
    ASSERT_TRUE(map->has_rule(pred2, succ2));
    ASSERT_EQ(2, map->size());
}


TEST_F(RuleBufferTest, revert_erase_predecessor)
{
    // BUFFER
    // dup pred succ
    //  t  A    0
    //  t  B    1

    buffer->erase(buffer->begin());
    // BUFFER
    // dup pred succ
    //  t  B    1

    buffer->revert();

    ASSERT_TRUE(has_rule(buffer, pred1, succ1));
    ASSERT_TRUE(has_rule(buffer, pred2, succ2));
    ASSERT_EQ(2, buffer->size());

    ASSERT_TRUE(map->has_rule(pred1, succ1));
    ASSERT_TRUE(map->has_rule(pred2, succ2));
    ASSERT_EQ(2, map->size());
}


TEST_F(RuleBufferTest, revert_change_predecessor)
{
    // BUFFER
    // dup pred succ
    //  t  A    0
    //  t  B    1

    buffer->change_predecessor(buffer->begin(), pred2);
    // BUFFER
    // dup pred succ
    //  f  B    0
    //  t  B    1

    buffer->revert();

    ASSERT_TRUE(has_rule(buffer, pred1, succ1));
    ASSERT_TRUE(has_rule(buffer, pred2, succ2));
    ASSERT_FALSE(has_rule(buffer, pred2, succ1));

    ASSERT_TRUE(map->has_rule(pred1, succ1));
    ASSERT_TRUE(map->has_rule(pred2, succ2));
    ASSERT_EQ(2, map->size());
}


TEST_F(RuleBufferTest, revert_remove_predecessor)
{
    // BUFFER
    // dup pred succ
    //  t  A    0
    //  t  B    1

    buffer->remove_predecessor(buffer->begin());
    // BUFFER
    // dup pred succ
    //  t  \0   0
    //  t  B    1

    buffer->revert();

    ASSERT_FALSE(has_rule(buffer, pred1, succ1));
    ASSERT_TRUE(has_rule(buffer, pred2, succ2));

    ASSERT_FALSE(map->has_rule(pred1, succ1));
    ASSERT_TRUE(map->has_rule(pred2, succ2));
    ASSERT_EQ(2, map->size());
}


TEST_F(RuleBufferTest, revert_change_successor)
{
    // BUFFER
    // dup pred succ
    //  t  A    0
    //  t  B    1

    buffer->change_successor(buffer->begin(), pred2);
    // BUFFER
    // dup pred succ
    //  t  A    1
    //  t  B    1

    buffer->revert();

    ASSERT_TRUE(has_rule(buffer, pred1, succ1));
    ASSERT_TRUE(has_rule(buffer, pred2, succ2));
    ASSERT_FALSE(has_rule(buffer, pred1, succ2));

    ASSERT_TRUE(map->has_rule(pred1, succ1));
    ASSERT_TRUE(map->has_rule(pred2, succ2));
    ASSERT_EQ(2, map->size());
}

TEST_F(RuleBufferTest, validate)
{
    // BUFFER
    // dup pred succ
    //  t  A    0
    //  t  B    1

    buffer->change_predecessor(buffer->begin(), pred2);
    // BUFFER
    // dup pred succ
    //  f  B    0
    //  t  B    1

    buffer->validate();
    buffer->revert();

    ASSERT_TRUE(has_rule(buffer, pred2, succ1));
    ASSERT_TRUE(has_rule(buffer, pred2, succ2));
    ASSERT_FALSE(has_rule(buffer, pred1, succ1));
    ASSERT_TRUE(has_duplicate(buffer, buffer->begin()));
    
    ASSERT_TRUE(map->has_rule(pred2, succ2));
    ASSERT_EQ(1, map->size());
}

