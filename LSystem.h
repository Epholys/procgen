#ifndef L_SYSTEM_H
#define L_SYSTEM_H


#include <vector>
#include <string>
#include <unordered_map>
#include <iostream>
#include <algorithm>

namespace lsys
{
    
    // Simple L-system generation class. Starting from an axiom and
    // simple production rules, generate by iteration a result array
    // of character. This class is a simple variant of more general
    // L-systems: context-free (one generating symbol by rule) and
    // deterministic (at most one rule for each symbol).
    // Invariants:
    //   - If an axiom is defined at construction, 'cache_.at(0)' contains it at
    //   all time.
    //   - 'cache_' is coherent with the 'rules_'.
    class LSystem
    {
    public:
        // A 'production_rule' is a sole symbol associated with an
        // array of symbols. In an iteration, each symbol will be
        // replaced by its associated array. Symbols without a rule
        // (terminals) are replaced by themselves.  The rules are
        // contained in a hashmap for quick access during an
        // iteration.
        using production_rules = std::unordered_map<char, std::vector<char>>;

        // For ease of use, defines a hashmap only used in a
        // constructor with std::string initalization instead of
        // std::vector.
        using pretty_production_rules = std::unordered_map<char, std::string>;
        
        // Constructors
        LSystem() = default;
        LSystem(const std::vector<char>& ax, const production_rules& prod);

        // Prettier way to create a LSystem:
        //     { "F" , { 'F', "F+F" } }
        //  VS
        //     { {'F'}, { 'F', { 'F', '+', 'F' } } }
        LSystem(const std::string& ax, const pretty_production_rules& prod);

        // Getters and setters
        std::vector<char> get_axiom() const;
        production_rules  get_rules() const;
        std::unordered_map<int, std::vector<char>>
            get_cache() const;

        void set_axiom(const std::vector<char>& axiom);
    
        // Returns the result of the 'n'-th iteration of the L-System and cache
        // it as well as the transitional iterations.
        std::vector<char> produce(int n);
       
    private:
        // The production rules applied in each iteration.
        production_rules rules_ = {};

        // The cache of all calculated iterations and the axiom.
        // It contains all the iterations up to the highest iteration
        // calculated. It is clearly not optimized for memory
        // usage. However, this project emphasizes interactivity so
        // quickly swapping between different iterations of the same
        // L-System.
        std::unordered_map<int, std::vector<char>> cache_ = {};
    };

    // Convert a std::string to a std::vector<char>.
    std::vector<char> string_to_vec (const std::string& str);

    // Convert a std::vector<char> to a std::string.
    std::string vec_to_string (const std::vector<char> vec);
}

// Print a std::vector<char> like a std::string.
std::ostream& operator<< (std::ostream& stream, std::vector<char> vec);
   
#endif


