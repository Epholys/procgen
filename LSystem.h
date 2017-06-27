#ifndef L_SYSTEM_H
#define L_SYSTEM_H


#include <vector>
#include <string>
#include <unordered_map>
#include <iostream>

// Simple L-system generation class Starting from an axiom and simple
// production rules, generate by iteration a result array of
// character. This class is a simple variant of more general
// L-systems: context-free (one generating symbol by rule) and
// deterministic (at most one rule for each symbol).
class LSystem
{
public:
    // A rule is a sole symbol associated with an array of symbols.
    // In an iteration, each symbol will be replaced by its associated
    // array. Symbols without a rules (terminals) are replaced by
    // themselves.  The rules are contained in a hashmap for quick
    // access during an iteration.
    using production_rules = std::unordered_map<char, std::vector<char>>;

    // For ease of use, defines a temporary hashmap with std::string
    // initalization instead of std::vector.
    using pretty_production_rules = std::unordered_map<char, std::string>;
        
    // Constructors
    LSystem() = default;
    LSystem(const std::vector<char>& ax, const production_rules& prod);

    // Prettier way to create a LSystem:
    //     { "F" , { 'F', "F+F" } }
    //  VS
    //     { {'F'}, { 'F', { 'F', '+', 'F' } } }
    LSystem(const std::string& ax, const pretty_production_rules& prod);

    // // Getters and setters
    std::vector<char> get_axiom() const;
    // void set_axiom(const std::vector<char>& str);

    production_rules get_rules() const;
    // void set_rules(const production_rules& prod);
    // void add_rule(const rule& r);
    // void remove_rule(char c);

    // // Resets the current string.
    // // Called if the axiom or the production rules are modified.
    // void reset();

    // Get the current result.
    std::vector<char> get_result() const;
    
    // Iterate n_iter times and return the updated current result.
    std::vector<char> iter(unsigned int n_iter = 1);
       
private:
    // The starting point defining the initial state.
    std::vector<char> axiom = {};

    // The production rules applied in each iteration.
    production_rules rules = {};

    // The current result.
    // Is always coherent with the axiom and the rules.
    std::vector<char> current = {};
};

std::ostream& operator<< (std::ostream& stream, std::vector<char> vec);
std::vector<char> string_to_vec (const std::string& str);

#endif


