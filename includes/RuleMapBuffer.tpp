template <typename Target>
RuleMapBuffer<Target>::RuleMapBuffer(std::shared_ptr<Target> target)
    : Observer<Target>(target)
    , buffer_{}
    , instruction_ {nullptr}
    , reverse_instruction_{nullptr}
    , is_notification_origin_{false}
 {
     this->add_callback([this]() { sync(); });

     // Initialize the buffer with the LSystem's rules.
     // By construction, there are not duplicate rules in a 'LSystem', so
     // there is no check: all rules are valid.
     for (const auto &rule : observer_target().get_rules()) {
         buffer_.push_back({true, rule.first, rule.second});
     }
 }

template<typename Target>
RuleMapBuffer<Target>::RuleMapBuffer(const RuleMapBuffer& other)
    : Observer<Target>(other.Observer<Target>::get_target())
    , buffer_ {other.buffer_}
    , instruction_ {nullptr}
    , reverse_instruction_ {nullptr}
    , is_notification_origin_{false}
{
    Observer<Target>::add_callback([this](){sync();});
}

template<typename Target>
RuleMapBuffer<Target>::RuleMapBuffer(const RuleMapBuffer& other, std::shared_ptr<Target> target)
    : Observer<Target>(target)
    , buffer_ {other.buffer_}
    , instruction_ {nullptr}
    , reverse_instruction_ {nullptr}
    , is_notification_origin_{false}
{
    Observer<Target>::add_callback([this](){sync();});
}

template<typename Target>
RuleMapBuffer<Target>::RuleMapBuffer(RuleMapBuffer&& other)
    : Observer<Target>(std::move(other.Observer<Target>::get_target()))
    , buffer_ {std::move(other.buffer_)}
    , instruction_ {nullptr}
    , reverse_instruction_ {nullptr}
    , is_notification_origin_{false}
{
    Observer<Target>::add_callback([this](){sync();});

    other.set_target(nullptr);
    other.buffer_ = {};
    other.instruction_ = nullptr;
    other.reverse_instruction_ = nullptr;
    other.is_notification_origin_ = false;
}

template<typename Target>
RuleMapBuffer<Target>& RuleMapBuffer<Target>::operator=(const RuleMapBuffer& other)
{
    if (this != &other)
    {
        Observer<Target>::set_target(other.Observer<Target>::get_target());
        buffer_ = other.buffer_;
        instruction_ = nullptr;
        reverse_instruction_ = nullptr;
        is_notification_origin_ = false;
            
        Observer<Target>::add_callback([this](){sync();});
    }
    return *this;
}

template<typename Target>
RuleMapBuffer<Target>& RuleMapBuffer<Target>::operator=(RuleMapBuffer&& other)
{
    if (this != &other)
    {
        Observer<Target>::set_target(std::move(other.Observer<Target>::get_target()));
        buffer_ = std::move(other.buffer_);
        instruction_ = nullptr;
        reverse_instruction_ = nullptr;
        other.is_notification_origin_ = false;

        Observer<Target>::add_callback([this](){sync();});

        other.set_target(nullptr);
        other.buffer_ = {};
        other.instruction_ = nullptr;
        other.reverse_instruction_ = nullptr;
        other.is_notification_origin_ = false;
    }
    return *this;
}

template<typename Target>
Target& RuleMapBuffer<Target>::observer_target() const
{
    return *(this->get_target());
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
    reverse_instruction_ = nullptr;
    
    // Add a scratch buffer: a valid empty rule.
    buffer_.push_back({});
}


template<typename Target>
void RuleMapBuffer<Target>::erase(const_iterator cit)
{
    Expects(cit != buffer_.end());

    reverse_instruction_ = nullptr;
    
    auto is_valid = cit->is_duplicate;
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

    reverse_instruction_ = nullptr;
    std::function<void()> part1 = []() {};
    std::function<void()> part2 = []() {};
    std::function<void()> part3 = []() {};
    
    // If the new predecessor is null, remove it.
    if (pred == '\0')
    {
        remove_predecessor(cit);
        return;
    }


    // There is 2 cases for the new rule:
    //  1. The new rule is valid/original => add it to the RuleMap.
    //  2. The new rule is a duplicate => do nothing
    // There is 4 cases for the old rule at 'cit':
    //  3. It was a duplicate rule => do nothing
    //  4. It was an original rule without a duplicate => remove it
    //  5. It was an original rule with a duplicate => replace it
    //  6. It was a scratch buffer => do nothing

    // Note: the order is important: 'add_rule()' for the new rule must
    // called before the others.
    bool old_was_original = cit->is_duplicate;
    auto old_pred = cit->predecessor; 
    auto old_succ = cit->successor;
    
    // Check if the new rule is original by finding an existing one with the
    // new predecessor.
    bool new_is_original = std::find_if(buffer_.cbegin(), buffer_.cend(),
                                        [pred](const auto& rule)
                                        { return rule.predecessor == pred &&
                                          rule.is_duplicate; }) == buffer_.end();
    const succ& succ = cit->successor;

    // Modify 'buffer_' with the new predicate.
    RuleMapBuffer<Target>::iterator it = remove_const(cit);
    *it = { new_is_original, pred, succ };
    part1 = [=]() { *it = { old_was_original, old_pred, old_succ }; };

    if (new_is_original) // Case 1.
    {
        is_notification_origin_= true;
        observer_target().add_rule(pred, succ);

        part2 = [=]()
            {
                is_notification_origin_= true;
                observer_target().remove_rule(pred);
            };
    }
    else // Case 2.
    {
    }
        
    if (old_was_original && old_pred != '\0')
    {
        is_notification_origin_= true;
        
        // Try to find a duplicate of the old rule
        auto old_duplicate = std::find_if(buffer_.begin(), buffer_.end(),
                                          [old_pred](const auto& rule)
                                          { return rule.predecessor == old_pred &&
                                            !rule.is_duplicate; });

        if (old_duplicate == buffer_.end()) // Case 4.
        {
            // Note: it is not necessary to call 'this->remove_rule()' as we
            // already checked that the old rule did not have a duplicate.
            observer_target().remove_rule(old_pred);
            part3 = [=]()
                {
                    is_notification_origin_ = true;
                    observer_target().add_rule(old_pred, old_succ);
                };
        }
        else // Case 5.
        {
            old_duplicate->is_duplicate = true;
            observer_target().add_rule(old_duplicate->predecessor,
                                       old_duplicate->successor);

            part3 = [=]()
                {
                    is_notification_origin_ = true;
                    observer_target().add_rule(old_duplicate->predecessor,
                                               old_succ);
                    old_duplicate->is_duplicate = false;
                };
        }
    }
    else // Case 3. & 6.
    {
        // Do nothing
    }

    reverse_instruction_ = [=]()
        {
            part3();
            part2();
            part1();
        };
}
    
template<typename Target>
void RuleMapBuffer<Target>::remove_predecessor(RuleMapBuffer<Target>::const_iterator cit)
{
    Expects(cit != buffer_.end());

    reverse_instruction_ = nullptr;
    
    // Turn '*cit' into a scratch buffer
        
    auto old_pred = cit->predecessor;
    auto is_valid = cit->is_duplicate;

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

    reverse_instruction_ = nullptr;
    std::function<void()> part1 = []() {};
    std::function<void()> part2 = []() {};

    auto old_succ = cit->successor;
    auto it = remove_const(cit);
    auto is_valid = cit->is_duplicate;
    auto pred = cit->predecessor;
    *it = { is_valid, pred, succ };
    part1 = [=]() { *it = {is_valid, pred, old_succ}; };

    // If it was an original rule, replace it in the Target
    if (is_valid && pred != '\0') {
        is_notification_origin_ = true;
        observer_target().add_rule(pred, succ);

        part2 = [=]()
            {
                is_notification_origin_ = true;
                observer_target().add_rule(pred, old_succ);
            };
    }

    reverse_instruction_ = [=]()
        {
            part2();
            part1();
        };
}

template <typename Target>
void RuleMapBuffer<Target>::remove_rule(char pred) {
    reverse_instruction_ = nullptr;

    // This method is called instead of 'observer_target().remove_rule(pred)'
    // to replace the old rule by a duplicate that is in priority in the same
    // RuleMapBuffer.

    // If 'pred' designate a still valid rule, remove this rule.
    auto original = std::find_if(
        buffer_.begin(), buffer_.end(), [pred](const auto &rule) {
            return rule.predecessor == pred && rule.is_duplicate;
        });
    if (original != buffer_.end()) {
        buffer_.erase(original);
    }

    // Find a duplicate
    auto duplicate = std::find_if(buffer_.begin(), buffer_.end(),
                                  [pred](const auto& rule)
                                  { return rule.predecessor == pred &&
                                    !rule.is_duplicate; });

    is_notification_origin_= true;
    if (duplicate != buffer_.end())
    {
        // If found, replace the old rule by it.
        duplicate->is_duplicate = true;
        observer_target().add_rule(pred, duplicate->successor);
    }
    else
    {
        // Otherwise, simply remove the rule.
        observer_target().remove_rule(pred);
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
void RuleMapBuffer<Target>::revert()
{
    if (reverse_instruction_)
    {
        reverse_instruction_();
        reverse_instruction_= nullptr;
    }
}

// Confirms the change, reset 'reverse_instruction_'
template<typename Target>
void RuleMapBuffer<Target>::validate()
{
    reverse_instruction_ = nullptr;
}

template<typename Target>
void RuleMapBuffer<Target>::sync()
{
    if (is_notification_origin_)
    {
        is_notification_origin_ = false;
        return;
    }

    buffer_.clear();

    for (const auto& rule : observer_target().get_rules())
    {
        buffer_.push_back({true, rule.first, rule.second});
    }
}
