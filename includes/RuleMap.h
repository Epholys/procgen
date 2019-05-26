#ifndef RULE_MAP_H
#define RULE_MAP_H


#include <unordered_map>

#include <gsl/gsl>

#include "Observable.h"

// RuleMap is a thin wrapper around an unordered_map specialized for the rules
// 'char -> T'. Semantically, in this project, it links a symbol of a LSystem
// (predecessor) with a successor that can be a string in a LSystem or an order
// for a Turtle in InterpretationMap.
// The rational behind its existence is to complete the classic methods of an
// unordered_map with the Observable behaviour. Moreover, it is useful has a
// base class for all '*Buffer' for the GUI.
template<typename Successor>
class RuleMap : public Observable
{
public:
    using successor = Successor;
    using rule = std::pair<char, Successor>;
    using rule_map = std::unordered_map<char, Successor>;
    
    RuleMap() = default;
    virtual ~RuleMap() {}
    explicit RuleMap(const rule_map& rules);
    RuleMap(std::initializer_list<typename rule_map::value_type> init);
    RuleMap(const RuleMap& other) = default;
    RuleMap(RuleMap&& other) = default;
    RuleMap& operator=(const RuleMap& other) = default;
    RuleMap& operator=(RuleMap&& other) = default;

    // --- Getters and setters ---
    // Check if 'predecessor' exists in the rules.
    bool has_predecessor(char predecessor) const;

    // Check if the rule "predecessor -> successor" exists.
    bool has_rule(char predecessor, const Successor& successor) const;

    // Get the rule associated with 'predecessor'
    // Exceptions:
    //   - Precondition: a production rule with 'predecessor' as a predecessor
    //   exists.
    rule get_rule(char predecessor) const;

    // Get all the rules
    const rule_map&  get_rules() const;

    // Get the size
    std::size_t size() const;
    
    // Add the rule "predecessor -> successor"
    // Note: replace the successor of an existing rule if 'predecessor' has
    // already a rule associated.
    virtual void add_rule(char predecessor, const Successor& successor);

    // Remove the rule associated to 'predecessor'
    // Exception:
    //   - Precondition: 'predecessor' must have a rule associated.
    virtual void remove_rule(char predecessor);

    // Clear the rules
    virtual void clear_rules();

protected:

    rule_map rules_ = {};
};

#include "RuleMap.tpp"


#endif
