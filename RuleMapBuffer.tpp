#include <algorithm>

template<typename Target>
RuleMapBuffer<Target>::RuleMapBuffer(const std::shared_ptr<Target>& target)
    : Observer<Target>(target)
    , target_ {*Observer<Target>::get_target()}
    , buffer_ {}
    , instruction_ {nullptr}
{
    this->add_callback([this](){sync();});
        
    // Initialize the buffer with the LSystem's rules.
    // By construction, there are not duplicate rules in a 'LSystem', so
    // there is no check: all rules are valid.
    for (const auto& rule : target_.get_rules())
    {
        buffer_.push_back({true, rule.first, rule.second});
    }
}

template<typename Target>
RuleMapBuffer<Target>::RuleMapBuffer(const RuleMapBuffer& other)
    : Observer<Target>(other.Observer<Target>::get_target())
    , target_ {other.target_}
    , buffer_ {other.buffer_}
    , instruction_ {nullptr}
{
    Observer<Target>::add_callback([this](){sync();});
}

template<typename Target>
RuleMapBuffer<Target>& RuleMapBuffer<Target>::operator=(RuleMapBuffer other)
{
    swap(other);
    return *this;
}

template<typename Target>
void RuleMapBuffer<Target>::swap(RuleMapBuffer& other)
{
    using std::swap;

    // Not a pure swap but Observer<T> must be manually swapped.
    const auto& tmp = Observer<Target>::get_target();
    this->set_target(other.Observer<Target>::get_target());
    other.Observer<Target>::set_target(tmp);
    
    this->add_callback([this](){sync();});
    other.Observer<Target>::add_callback([&other](){other.sync();});

    swap(target_, other.target_);
    swap(buffer_, other.buffer_);
    swap(instruction_, other.instruction_);
}


template<typename Target>
Target& RuleMapBuffer<Target>::get_target() const
{
    return target_;
}

template<typename Target>
typename RuleMapBuffer<Target>::const_iterator RuleMapBuffer<Target>::begin() const
{
    return buffer_.cbegin();
}

template<typename Target>
typename RuleMapBuffer<Target>::const_iterator RuleMapBuffer<Target>::end() const
{
    return buffer_.cend();
}

template<typename Target>
size_t RuleMapBuffer<Target>::size() const
{
    return buffer_.size();
}


template<typename Target>
typename RuleMapBuffer<Target>::iterator RuleMapBuffer<Target>::remove_const(const_iterator cit)
{
    // https://stackoverflow.com/a/10669041/4309005
    return buffer_.erase(cit, cit);
}

    
template<typename Target>
void RuleMapBuffer<Target>::add_rule()
{
    // Add a scratch buffer: a valid empty rule.
    buffer_.push_back({});
}


template<typename Target>
void RuleMapBuffer<Target>::erase(const_iterator cit)
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

template<typename Target>
void RuleMapBuffer<Target>::change_predecessor(const_iterator cit, char pred)
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
        target_.add_rule(pred, succ);
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
            target_.remove_rule(old_pred);
        }
        else // Case 5.
        {
            old_duplicate->validity = true;
            target_.add_rule(old_pred, old_duplicate->successor);
                
        }
    }
    else // Case 3. & 6.
    {
        // Do nothing
    }
}
    
template<typename Target>
void RuleMapBuffer<Target>::remove_predecessor(RuleMapBuffer<Target>::const_iterator cit)
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


template<typename Target>
void RuleMapBuffer<Target>::change_successor(const_iterator cit, const succ& succ)
{
    Expects(cit != buffer_.end());

    auto it = remove_const(cit);

    auto is_valid = cit->validity;
    auto pred = cit->predecessor;
    *it = { is_valid, pred, succ };

    // If it was an original rule, replace it in the Target
    if (is_valid && pred != '\0')
    {
        target_.add_rule(pred, succ);
    }
}

template<typename Target>
void RuleMapBuffer<Target>::remove_rule(char pred)
{
    // This method is called instead of 'target_.remove_rule(pred)' to replace
    // the old rule by a duplicate that is in priority in the same
    // RuleMapBuffer.

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
        target_.add_rule(pred, duplicate->successor);
    }
    else
    {
        // Otherwise, simply remove the rule.
        target_.remove_rule(pred);
    }
}
    
template<typename Target>
void RuleMapBuffer<Target>::delayed_add_rule()
{
    instruction_ = [=](){ add_rule(); };
}
template<typename Target>
void RuleMapBuffer<Target>::delayed_erase(const_iterator cit)
{
    instruction_ = [=](){ erase(cit); };
}
template<typename Target>
void RuleMapBuffer<Target>::delayed_change_predecessor(const_iterator cit, char pred)
{
    instruction_ = [=](){ change_predecessor(cit, pred); };
}
template<typename Target>
void RuleMapBuffer<Target>::delayed_remove_predecessor(const_iterator cit)
{
    instruction_ = [=](){ remove_predecessor(cit); };
}
template<typename Target>
void RuleMapBuffer<Target>::delayed_change_successor(const_iterator cit, const succ& succ)
{
    instruction_ = [=](){ change_successor(cit, succ); };
}

template<typename Target>
void RuleMapBuffer<Target>::apply()
{
    if(instruction_)
    {
        instruction_();
        instruction_ = nullptr;
    }
}

template<typename Target>
void RuleMapBuffer<Target>::sync()
{
    // This function is highly inefficient: the buffer and the Target are
    // updated even if there weren't any modification.

        
    const auto& rules = target_.get_rules();

    // First step: synchronize addition into the Target's rules.
    // For each rule of the Target, we check if it exists in the buffer. If
    // so, we update the successor. Otherwise, we add a new rule to the
    // buffer.
    for (const auto& rule : rules)
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

    // Second step: Sychronize deletion of the Target's rules:
    // For each rule of the buffer, we check if it exists in the Target. If
    // not, we remove it from the buffer.
    for (auto it = buffer_.begin(); it != buffer_.end(); )
    {
        if(it->validity && it->predecessor != '\0')
        {
            bool exist = rules.count(it->predecessor) > 0 ? true : false;
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

    // Final step: Synchronize rules between the RuleMapBuffers: if a rule
    // is removed in a RuleMapBuffer and if it does not have a duplicate in
    // the same RuleMapBuffer, we try to find a duplicate in an other
    // buffer.
    // To do so, if we spot in a buffer an invalid rule without a valid one, we
    // make it valid and update the LSystem (and so every other RuleMapBuffer).
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
                target_.add_rule(it->predecessor, it->successor);
            }
        }
    }
}
