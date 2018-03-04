#include <algorithm>
#include "LSystemBuffer.h"

namespace procgui
{
    LSystemBuffer::LSystemBuffer(const std::shared_ptr<LSystem>& lsys)
        : Observer<LSystem> {lsys}
        , lsys_ {*Observer<LSystem>::target_}
        , buffer_ {}
        , instruction_ {nullptr}
    {
        Observer<LSystem>::add_callback([this](){sync();});
        
        // Initialize the buffer with the LSystem's rules.
        // By construction, there are not duplicate rules in a 'LSystem', so
        // there is no check: all rules are valid.
        for (const auto& rule : lsys_.get_rules())
        {
            buffer_.push_back({true, rule.first, rule.second});
        }
    }

    LSystem& LSystemBuffer::get_lsys() const
    {
        return lsys_;
    }

    LSystemBuffer::const_iterator LSystemBuffer::begin() const
    {
        return buffer_.cbegin();
    }

    LSystemBuffer::const_iterator LSystemBuffer::end() const
    {
        return buffer_.cend();
    }

    LSystemBuffer::iterator LSystemBuffer::remove_const(const_iterator cit)
    {
        // https://stackoverflow.com/a/10669041/4309005
        return buffer_.erase(cit, cit);
    }

    
    void LSystemBuffer::add_rule()
    {
        // Add a scratch buffer: a valid empty rule.
        buffer_.push_back({true, '\0', "" });
    }


    void LSystemBuffer::erase(const_iterator cit)
    {
        Expects(cit != buffer_.end());
        
        auto is_valid = cit->validity;
        auto pred = cit->predecessor;

        // If the rule is valid and not a scratch buffer, removes it.
        if (is_valid && pred != '\0')
        {
            remove_rule(pred);
        }
        // Otherwise, simply remove it from the buffer.
        else
        {
            buffer_.erase(cit);
            return;
        }
    }

    void LSystemBuffer::change_predecessor(const_iterator cit, char pred)
    {
        Expects(cit != buffer_.end());

        // If the new predecessor is null, remove it.
        if (pred == '\0')
        {
            remove_predecessor(cit);
            return;
        }

        // There is 2 cases for the new rule:
        //  1. The new rule is valid/original => add it to the LSystem.
        //  2. The new rule is a duplicate => do nothing
        // There is 4 cases for the old rule at 'cit': 
        //  3. It was a duplicate rule => do nothing
        //  4. It was an original rule without a duplicate => remove it
        //  5. It was an original rule with a duplicate => replace it
        //  6. It was a scratch buffer => do nothing

        // Note: the order is important: 'add_rule()' for the new rule must
        // called before the others.


        bool old_was_original = cit->validity;
        auto old_pred = cit->predecessor;

        // Check if the new rule is original by finding an existing one with the
        // new predecessor.
        bool new_is_original = std::find_if(buffer_.cbegin(), buffer_.cend(),
                                            [pred](const auto& rule)
                                            { return rule.predecessor == pred &&
                                              rule.validity; }) == buffer_.end();
        const succ& succ = cit->successor;

        // Modify 'buffer_' with the new predicate.
        *remove_const(cit) = { new_is_original, pred, succ };

        if (new_is_original) // Case 1.
        {
            lsys_.add_rule(pred, succ);
        }
        else // Case 2.
        {
            // Do nothing
        }
        
        if (old_was_original && old_pred != '\0')
        {
            // Try to find a duplicate of the old rule
            auto old_duplicate = std::find_if(buffer_.begin(), buffer_.end(),
                                              [old_pred](const auto& rule)
                                              { return rule.predecessor == old_pred &&
                                                !rule.validity; });

            if (old_duplicate == buffer_.end()) // Case 4.
            {
                // Note: it is not necessary to call 'this->remove_rule()' as we
                // already checked that the old rule did not have a duplicate.
                lsys_.remove_rule(old_pred);
            }
            else // Case 5.
            {
                old_duplicate->validity = true;
                lsys_.add_rule(old_pred, old_duplicate->successor);
                
            }
        }
        else // Case 3. & 6.
        {
            // Do nothing
        }
    }
    
    void LSystemBuffer::remove_predecessor(LSystemBuffer::const_iterator cit)
    {
        Expects(cit != buffer_.end());

        // Turn '*cit' into a scratch buffer
        
        auto old_pred = cit->predecessor;
        auto is_valid = cit->validity;

        auto it = remove_const(cit);
        const succ& succ = cit->successor;
        *it = { true, '\0', succ };

        // If it was an original rule, remove it.
        if (is_valid)
        {
            remove_rule(old_pred);
        }
    }


    void LSystemBuffer::change_successor(const_iterator cit, const succ& succ)
    {
        Expects(cit != buffer_.end());

        auto it = remove_const(cit);

        auto is_valid = cit->validity;
        auto pred = cit->predecessor;
        *it = { is_valid, pred, succ };

        // If it was an original rule, replace it in the LSystem
        if (is_valid && pred != '\0')
        {
            lsys_.add_rule(pred, succ);
        }
    }

    void LSystemBuffer::remove_rule(char pred)
    {
        // This method is called instead of 'lsys_.remove_rule(pred)' to replace
        // the old rule by a duplicate that is in priority in the same
        // LSystemBuffer.

        // If 'pred' designate a still valid rule, remove this rule.
        auto original = std::find_if(buffer_.begin(), buffer_.end(),
                                      [pred](const auto& rule)
                                      { return rule.predecessor == pred &&
                                               rule.validity; });
        if (original != buffer_.end())
        {
            buffer_.erase(original);
        }

        // Find a duplicate
        auto duplicate = std::find_if(buffer_.begin(), buffer_.end(),
                                      [pred](const auto& rule)
                                      { return rule.predecessor == pred &&
                                              !rule.validity; });
        if (duplicate != buffer_.end())
        {
            // If found, replace the old rule by it.
            duplicate->validity = true;
            lsys_.add_rule(pred, duplicate->successor);
        }
        else
        {
            // Otherwise, simply remove the rule.
            lsys_.remove_rule(pred);
        }
    }
    
    void LSystemBuffer::delayed_add_rule()
    {
        instruction_ = [=](){ add_rule(); };
    }
    void LSystemBuffer::delayed_erase(const_iterator cit)
    {
        instruction_ = [=](){ erase(cit); };
    }
    void LSystemBuffer::delayed_change_predecessor(const_iterator cit, char pred)
    {
        instruction_ = [=](){ change_predecessor(cit, pred); };
    }
    void LSystemBuffer::delayed_remove_predecessor(const_iterator cit)
    {
        instruction_ = [=](){ remove_predecessor(cit); };
    }
    void LSystemBuffer::delayed_change_successor(const_iterator cit, const succ& succ)
    {
        instruction_ = [=](){ change_successor(cit, succ); };
    }

    void LSystemBuffer::apply()
    {
        if(instruction_)
        {
            instruction_();
            instruction_ = nullptr;
        }
    }

    void LSystemBuffer::sync()
    {
        // This function is highly inefficient: the buffer and the LSystem are
        // updated even if there weren't any modification.

        
        const auto& lsys_rules = lsys_.get_rules();

        // First step: synchronize addition int the LSystem's rules.
        // For each rule of the LSystem, we check if it exists in the buffer. If
        // so, we update the successor. Otherwise, we add a new rule to the
        // buffer.
        for (const auto& rule : lsys_rules)
        {
            char pred = rule.first;
            auto it = std::find_if(buffer_.begin(), buffer_.end(),
                                   [pred](const auto& rule)
                                   { return rule.predecessor == pred &&
                                     rule.validity; });
            if (it != buffer_.end())
            {
                it->successor = rule.second;
            }
            else
            {
                buffer_.push_back({true, rule.first, rule.second});
            }
        }

        // Second step: Sychronize deletion of the LSystem's rules:
        // For each rule of the buffer, we check if it exists in the LSystem. If
        // not, we remove it from the buffer.
        for (auto it = buffer_.begin(); it != buffer_.end(); )
        {
            if(it->validity && it->predecessor != '\0')
            {
                bool exist = lsys_rules.count(it->predecessor) > 0 ? true : false;
                if (!exist)
                {
                    it = buffer_.erase(it);
                }
                else
                {
                    ++it;
                }
            }
            else
            {
                ++it;
            }
        }

        // Final step: Synchronize rules between the LSystemBuffers: if a rule
        // is removed in a LSystemBuffer and if it does not have a duplicate in
        // the same LSystemBuffer, we try to find a duplicate in an other
        // buffer.
        // To do so, if we spot in a buffer an invalid rule without a valid one,
        // we make it valid and update the LSystem (and so every other
        // LSystemBuffer).
        for (auto it = buffer_.begin(); it != buffer_.end(); ++it)
        {
            if (!it->validity)
            {
                auto original = find_duplicate_if(it, buffer_.begin(), buffer_.end(),
                                                  [](const auto& e1, const auto& e2)
                                                  { return e1.predecessor == e2.predecessor &&
                                                           e2.validity; });
                if (original == buffer_.end())
                {
                    it->validity = true;
                    lsys_.add_rule(it->predecessor, it->successor);
                }
            }
        }
    }
}
