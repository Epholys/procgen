#ifndef L_SYSTEM_H
#define L_SYSTEM_H


#include <string>
#include <unordered_map>
#include <iostream>
#include <algorithm>

#include "Observable.h"

// Simple L-system generation class. Starting from an axiom and
// simple production rules, generate by iteration a result array
// of character. This class is a simple variant of more general
// L-systems: context-free (one generating symbol by rule) and
// deterministic (at most one rule for each symbol).
// Invariants:
//   - If an axiom is defined at construction, 'cache_.at(0)' contains it at
//   all time.
//   - 'cache_' is coherent with the 'rules_'.
class LSystem : public Observable
{
public:
    // A 'production_rule' is a sole symbol associated with an
    // array of symbols. In an iteration, each symbol will be
    // replaced by its associated array. Symbols without a rule
    // (terminals) are replaced by themselves.  The rules are
    // contained in a hashmap for quick access during an
    // iteration.
    using rule = std::pair<char, std::string>;
    using production_rules = std::unordered_map<char, std::string>;
        
    // Constructors
    LSystem() = default;
    LSystem(const std::string& axiom, const production_rules& prod);

    // Getters and setters
    std::string get_axiom() const;
    const production_rules&  get_rules() const;
    rule get_rule(char predecessor) const;
    const std::unordered_map<int, std::string>& get_cache() const;

    void set_axiom(const std::string& axiom);
    void add_rule(char predecessor, const std::string& successor);
    void remove_rule(char predecessor);
    void clear_rules();
        
    // Returns the result of the 'n'-th iteration of the L-System and cache
    // it as well as the transitional iterations.
    std::string produce(int n);
       
private:
    // The production rules applied in each iteration.
    production_rules rules_ = {};

    // The cache of all calculated iterations and the axiom.
    // It contains all the iterations up to the highest iteration
    // calculated. It is clearly not optimized for memory
    // usage. However, this project emphasizes interactivity so
    // quickly swapping between different iterations of the same
    // L-System.
    std::unordered_map<int, std::string> cache_ = {};
};
   
#endif



