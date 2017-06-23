#ifndef L_SYSTEM_H
#define L_SYSTEM_H


#include <string>
#include <unordered_map>

// Simple L-system generation class Starting from an axiom and simple
// production rules, generate by iteration a result string. Simple
// variant: context-free (one generating symbol by rule) and
// deterministic (at most one rule for each symbol).
class LSystem
{
public:
    // A rule is a sole symbol associated with an array of symbols.
    // In an iteration, each symbol will be replaced by its associated
    // array. Symbols without a rules (terminals) are replaced by
    // themselves.
    // The rules are contained in a hashmap for quick access during an
    // iteration.
    using production_rules = std::unordered_map<char, std::string>;
        
    // Constructors
    LSystem() = default;
    LSystem(const std::string& ax, const production_rules& prod);
        
    // Getters and setters
    std::string get_axiom() const;
    void set_axiom(const std::string& str);

    production_rules get_rules() const;
    void set_rules(const production_rules& prod);

    // Resets the current string.
    // Called if the axiom or the production rules are modified.
    void reset();

    // Get the current string.
    std::string get_result() const;
    
    // Iterate n_iter times and return the updated current string.
    std::string iter(unsigned int n_iter = 1);

    // May be used in the future:
    // // void add_rule(const rule& r);
    // // void remove_rule(char c);
       
private:
    // The starting point defining the initial state.
    std::string axiom = "";

    // The production rules applied in each iteration.
    production_rules rules = {};

    // The current string.
    // Is always coherent with the axiom and the rules.
    std::string current = "";
};


#endif


