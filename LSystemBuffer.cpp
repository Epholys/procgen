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

        // If the rule is valid and not a scratch buffer, removes it from the
        // 'LSystem'.
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

        // If the predecessor is null, removes it.
        if (pred == '\0')
        {
            remove_predecessor(cit);
            return;
        }


        bool old_was_original = cit->validity;
        auto old_pred = cit->predecessor;

        bool new_is_original = std::find_if(buffer_.cbegin(), buffer_.cend(),
                                            [pred](const auto& rule)
                                            { return rule.predecessor == pred &&
                                              rule.validity; }) == buffer_.end();
        
        const succ& succ = cit->successor;

        *remove_const(cit) = { new_is_original, pred, succ };

        if (new_is_original)
        {
            lsys_.add_rule(pred, succ);
        }
        if (old_was_original && old_pred != '\0')
        {
            auto old_duplicate = std::find_if(buffer_.begin(), buffer_.end(),
                                              [old_pred](const auto& rule)
                                              { return rule.predecessor == old_pred &&
                                                !rule.validity; });

            if (old_duplicate == buffer_.end())
            {
                lsys_.remove_rule(old_pred);
            }
            else
            {
                old_duplicate->validity = true;
                lsys_.add_rule(old_pred, old_duplicate->successor);
                
            }
        }
    }
    
    void LSystemBuffer::remove_predecessor(LSystemBuffer::const_iterator cit)
    {
        Expects(cit != buffer_.end());

        auto old_pred = cit->predecessor;
        auto is_valid = cit->validity;

        auto it = remove_const(cit);
        const succ& succ = cit->successor;
        *it = { true, '\0', succ };

        if (is_valid)
        {
            remove_rule(old_pred);
        }
    }


    void LSystemBuffer::change_successor(const_iterator cit, const succ& succ)
    {
        Expects(cit != buffer_.end());

        auto it = remove_const(cit);

        auto valid = cit->validity;
        auto pred = cit->predecessor;
        *it = { valid, pred, succ };

        if (valid)
        {
            lsys_.add_rule(pred, succ);
        }
    }

    void LSystemBuffer::remove_rule(char pred)
    {
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
            // If found, make it the next rule
            duplicate->validity = true;
            lsys_.add_rule(pred, duplicate->successor);
        }
        else
        {
            // If not found, simply remove the rule
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
        const auto& lsys_rules = lsys_.get_rules();

        // gérer les ajouts
        // gérer les suppressions
        // gérer les modifications
        // gérer les \0
        // gérer les non valides

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

        for (auto it = buffer_.begin(); it != buffer_.end(); ++it)
        {
            if (!it->validity)
            {
                auto original = find_duplicate_if(it, buffer_.begin(), buffer_.end(),
                                                  [](const auto& e1, const auto& e2)
                                                  { return e1.predecessor == e2.predecessor &&
                                                           e2.validity; });
                bool is_duplicate = original != buffer_.end();
                if (!is_duplicate)
                {
                    it->validity = true;
                    lsys_.add_rule(it->predecessor, it->successor);
                }
            }
        }
    }
}
