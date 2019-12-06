template<typename Successor>
RuleMap<Successor>::RuleMap(const Rules& rules)
    : Observable{}
    , rules_(rules)
{
}

template<typename Successor>
RuleMap<Successor>::RuleMap(std::initializer_list<typename Rules::value_type> init)
    : Observable{}
    , rules_{init}
{
}


template<typename Successor>
bool RuleMap<Successor>::has_predecessor(char predecessor) const
{
    return rules_.count(predecessor) > 0;
}

template<typename Successor>
bool RuleMap<Successor>::has_rule(char predecessor, const Successor& successor) const
{
    return has_predecessor(predecessor) && rules_.at(predecessor) == successor;
}

template<typename Successor>
typename RuleMap<Successor>::Rule RuleMap<Successor>::get_rule(char predecessor) const
{
    Expects(has_predecessor(predecessor));
    return { predecessor, rules_.at(predecessor) };
}

template<typename Successor>
const typename RuleMap<Successor>::Rules& RuleMap<Successor>::get_rules() const
{
    return rules_;
}
template<typename Successor>
std::size_t RuleMap<Successor>::size() const
{
    return rules_.size();
}


template<typename Successor>
void RuleMap<Successor>::add_rule(char predecessor, const Successor& successor)
{
    rules_[predecessor] = successor;
    notify();
}

template<typename Successor>
void RuleMap<Successor>::remove_rule(char predecessor)
{
    auto rule = rules_.find(predecessor);
    Expects(rule != rules_.end());

    rules_.erase(rule);
    notify();
}

template<typename Successor>
void RuleMap<Successor>::clear_rules()
{
    rules_.clear();
    notify();
}

template<typename Successor>
void RuleMap<Successor>::replace_rules(const Rules& new_rules)
{
    rules_ = new_rules;
    notify();
}
