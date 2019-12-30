template<typename Target>
RuleMapBuffer<Target>::RuleMapBuffer(const Target& rule_map)
    : rule_map_ {rule_map}
{
    // Initialize the buffer with the LSystem's rules.
    // By construction, there are not duplicate rules in a 'LSystem', so
    // there is no check: all rules are valid.
    for (const auto& rule : rule_map_.get_rules())
    {
        buffer_.push_back({true, rule.first, rule.second});
    }
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
const Target& RuleMapBuffer<Target>::get_rule_map() const
{
    return rule_map_;
}
template<typename Target>
Target& RuleMapBuffer<Target>::ref_rule_map()
{
    return rule_map_;
}

template<typename Target>
void RuleMapBuffer<Target>::set_rule_map(const Target& new_rule_map)
{
    rule_map_ = new_rule_map;
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
    previous_buffer_ = buffer_;

    // Add a scratch buffer: a valid empty rule.
    buffer_.push_back({});
}


template<typename Target>
void RuleMapBuffer<Target>::erase(const_iterator cit)
{
    Expects(cit != buffer_.end());

    previous_buffer_ = buffer_;

    // If the rule is valid and not a scratch buffer, removes it.
    if (cit->predecessor != '\0' && cit->is_active)
    {
        remove_rule(cit->predecessor);
    }
    // Otherwise, simply remove it from the buffer.
    else
    {
        buffer_.erase(cit);
    }
}

template<typename Target>
void RuleMapBuffer<Target>::change_predecessor(const_iterator cit, char pred)
{
    Expects(cit != buffer_.end());

    previous_buffer_ = buffer_;

    // If the new predecessor is null, remove it.
    if (pred == '\0')
    {
        remove_predecessor(cit);
        return;
    }

    // There is 2 cases for the new rule:
    //  1. The new rule is active => add it to the RuleMap.
    //  2. The new rule is a duplicate => do nothing
    // There is 4 cases for the old rule at 'cit':
    //  3. It was a duplicate rule => do nothing
    //  4. It was an original rule without a duplicate => remove it
    //  5. It was an original rule with a duplicate => replace it
    //  6. It was a scratch buffer => do nothing

    auto [previous_was_active, previous_pred, succ] = *cit;

    // Check if the new rule is original by finding an existing one with the
    // new predecessor.
    bool new_is_original = std::find_if(buffer_.cbegin(),
                                        buffer_.cend(),
                                        [pred](const auto& rule) {
                                            return rule.predecessor == pred && rule.is_active;
                                        })
                           == buffer_.end();

    // Modify 'buffer_' with the new predicate.
    RuleMapBuffer<Target>::iterator it = remove_const(cit);

    *it = Rule({new_is_original, pred, succ});

    if (new_is_original) // Case 1.
    {
        rule_map_.add_rule(pred, succ);
    }
    else // Case 2.
    {
        // Do nothing
    }

    if (previous_was_active && previous_pred != '\0')
    {
        // Try to find a duplicate of the old rule
        auto old_duplicate =
            std::find_if(buffer_.begin(), buffer_.end(), [previous_pred](const auto& rule) {
                return rule.predecessor == previous_pred && !rule.is_active;
            });

        if (old_duplicate == buffer_.end()) // Case 4.
        {
            rule_map_.remove_rule(previous_pred);
        }
        else // Case 5.
        {
            old_duplicate->is_active = true;
            rule_map_.add_rule(old_duplicate->predecessor, old_duplicate->successor);
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

    previous_buffer_ = buffer_;

    auto was_active = cit->is_active;
    auto previous_predecessor = cit->predecessor;

    // Go back to scratch buffer.
    auto it = remove_const(cit);
    *it = {true, '\0', cit->successor};

    // If it was an original rule, remove it.
    if (was_active)
    {
        remove_rule(previous_predecessor);
    }
}


template<typename Target>
void RuleMapBuffer<Target>::change_successor(const_iterator cit, const Successor& succ)
{
    Expects(cit != buffer_.end());

    previous_buffer_ = buffer_;

    auto it = remove_const(cit);
    *it = Rule({cit->is_active, cit->predecessor, succ});

    // If it was an original rule, replace it in the Target
    if (cit->is_active && cit->predecessor != '\0')
    {
        rule_map_.add_rule(cit->predecessor, succ);
    }
}

template<typename Target>
void RuleMapBuffer<Target>::remove_rule(char pred)
{
    previous_buffer_ = buffer_;

    // This method is called instead of 'rule_map_->remove_rule(pred)'
    // to replace the old rule by a duplicate that is in priority in the same
    // RuleMapBuffer.

    // If 'pred' designate a still valid rule, remove this rule.
    auto original = std::find_if(buffer_.begin(), buffer_.end(), [pred](const auto& rule) {
        return rule.predecessor == pred && rule.is_active;
    });
    if (original != buffer_.end())
    {
        buffer_.erase(original);
    }

    // Find a duplicate
    auto duplicate = std::find_if(buffer_.begin(), buffer_.end(), [pred](const auto& rule) {
        return rule.predecessor == pred && !rule.is_active;
    });

    if (duplicate != buffer_.end())
    {
        duplicate->is_active = true;
        // If found, replace the old rule by it.
        rule_map_.add_rule(pred, duplicate->successor);
    }
    else
    {
        // Otherwise, simply remove the rule.
        rule_map_.remove_rule(pred);
    }
}

template<typename Target>
typename Target::Rules RuleMapBuffer<Target>::generate_rule_map()
{
    typename Target::Rules map;

    for (const auto& rule : buffer_)
    {
        if (rule.is_active)
        {
            map[rule.predecessor] = rule.successor;
        }
    }

    return map;
}

template<typename Target>
bool RuleMapBuffer<Target>::poll_modification()
{
    // RuleMapBuffer<>'s own internal modifications does not matter, it is just
    // a wrapper for the GUI. So, we forward the modifications from the
    // underlying RuleMap.
    return rule_map_.poll_modification();
}

template<typename Target>
void RuleMapBuffer<Target>::revert()
{
    if (!previous_buffer_.empty())
    {
        buffer_ = previous_buffer_;

        rule_map_.replace_rules(generate_rule_map());

        previous_buffer_.clear();
    }
}

// Confirms the change, reset 'reverse_instruction_'
template<typename Target>
void RuleMapBuffer<Target>::validate()
{
    previous_buffer_.clear();
}
