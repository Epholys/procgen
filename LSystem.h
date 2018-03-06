#ifndef L_SYSTEM_H
#define L_SYSTEM_H


#include <string>
#include <unordered_map>
#include <iostream>
#include <algorithm>

#include "Observable.h"
#include "RuleMap.h"

// Simple L-system generation class. Starting from an axiom and
// simple production rules, generate by iteration a result array
// of character. This class is a simple variant of more general
// L-systems: context-free (one generating symbol by rule) and
// deterministic (at most one rule for each symbol).
// Invariants:
//   - If an axiom is defined at construction, 'cache_.at(0)' contains it at
//   all time.
//   - 'cache_' is coherent with the 'rules_'.
class LSystem : public RuleMap<std::string>
{
public:
    // A 'production_rule' is a sole symbol associated with an
    // array of symbols. In an iteration, each symbol will be
    // replaced by its associated array. Symbols without a rule
    // (terminals) are replaced by themselves.  The rules are
    // contained in a hashmap for quick access during an
    // iteration.
    using production_rules = RuleMap::rule_map;
        
    // Constructors
    LSystem() = default;
    LSystem(const std::string& axiom, const production_rules& prod);

    // --- Getters and setters ---
    // Get the axiom.
    std::string get_axiom() const;

    // Get the cache
    const std::unordered_map<int, std::string>& get_cache() const;

    // Set the axiom to 'axiom'
    void set_axiom(const std::string& axiom);

    // Add the rule "predecessor -> successor"
    // Note: replace the successor of an existing rule if 'predecessor' has
    // already a rule associated.
    void add_rule(char predecessor, const RuleMap::successor& successor) override;

    // Remove the rule associated to 'predecessor'
    // Exception:
    //   - Precondition: 'predecessor' must have a rule associated.
    void remove_rule(char predecessor) override;

    // Clear the rules
    void clear_rules() override;

    // Returns the result of the 'n'-th iteration of the L-System and cache
    // it as well as the transitional iterations.
    //
    // Exceptions:
    //   - Precondition: n positive.
    //   - Throw in case of allocation problem.
    //   - Throw at '.at()' if code is badly refactored.
    std::string produce(int n);
       
private:
    // The cache of all calculated iterations and the axiom.
    // It contains all the iterations up to the highest iteration
    // calculated. It is clearly not optimized for memory
    // usage. However, this project emphasizes interactivity so
    // quickly swapping between different iterations of the same
    // L-System.
    std::unordered_map<int, std::string> cache_ = {};
};
   
#endif

