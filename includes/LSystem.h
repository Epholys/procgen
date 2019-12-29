#ifndef L_SYSTEM_H
#define L_SYSTEM_H


#include <string>
#include <unordered_map>

#include "cereal/cereal.hpp"
#include "cereal/types/unordered_map.hpp"

#include "types.h"
#include "RuleMap.h"
#include "LoadMenu.h"

namespace cereal
{
    // Saving for std::map<std::string, std::string> for text based archives
    // Copied from https://uscilab.github.io/cereal/archive_specialization.html (MIT License)
    template <class Archive, class C, class A,
              traits::EnableIf<traits::is_text_archive<Archive>::value> = traits::sfinae> inline
    void save( Archive & ar, std::unordered_map<char, std::string, C, A> const& map )
    {
        for(const auto& i : map)
            ar(cereal::make_nvp(std::string()+i.first, i.second));
    }

    // Loading for std::map<char, std::string> for text based archives
    template <class Archive, class C, class A,
              traits::EnableIf<traits::is_text_archive<Archive>::value> = traits::sfinae> inline
    void load( Archive & ar, std::unordered_map<char, std::string, C, A>& map)
    {
        map.clear();

        bool key_too_big = false;
        bool void_key = false;
        auto hint = map.begin();
        while(true)
        {
            const auto namePtr = ar.getNodeName();

            if(!namePtr)
                break;

            std::string loaded_key = namePtr;
            std::string value; ar(value);
            char key = ' ';
            if (loaded_key.size() != 0)
            {
                if (loaded_key.size() > 1)
                {
                    key_too_big = true;
                }
                key = loaded_key.at(0);
                hint = map.emplace_hint(hint, key, std::move(value));
            }
            else
            {
                void_key = true;
            }
        }

        if (key_too_big)
        {
            controller::LoadMenu::add_loading_error_message("One or more LSystem's key was too big, it is now cropped.");
        }
        if (void_key)
        {
            controller::LoadMenu::add_loading_error_message("One or more LSystem's key was empty, so it was ignored.");
        }

    }
}

// Simple L-system generation class. Starting from an axiom and simple
// production rules, generate by iteration a result array of character by
// replacing each character predecessor by its word successor. It also produces
// an array containing for some selected characters the derivation count
// corresponding to the number of iteration of these characters was derived from
// any rule.
//
// For example:
//   axiom: "F"
//   production rules:
//     "F" -> "F+G" (<- predecessor to save iteration count)
//     "G" -> "F-G"
//   iteration 0:
//     production_array: "F"
//     iteration_array:  {0}
//   iteration 1:
//     production_array: "F + G" (without space)
//     iteration_array:  {1,1,1}
//   iteration 2:
//     production_array: "F + G  +  F - G" (without space)
//     iteration_array:  {2,2,2, 1, 1,1,1}
//     max_iteration: 2
//
// This class is a simple variant of more general
// L-systems: context-free (one generating symbol by rule) and
// deterministic (at most one rule for each symbol).
// Invariants:
//   - If an axiom is defined at construction, 'production_cache_.at(0)'
//   contains it at all time. And 'iteration_count_cache_.at(0)' is 0.
//   - 'iteration_count_cache_' is coherent with 'iteraction_predecessors_'
//   - 'production_cache_' and 'iteration_count_cache_' are coherent with the
//   rules inherited from 'RuleMap<std::string>
//   - As a consequence, 'iteration_count_cache_' and 'production_cache_' are
//   coherent BUT 'iteration_count_cache_' may have less elements than
//   'production_cache_'. In any case, 'iteration_count_cache_' has always less
//   or equal element than production_cache_'.
//
//   The production rules are managed by the parent class
//   RuleMap<std::string>. Its methodss are overloaded to respect the invariants
//   of this class.
class LSystem : public RuleMap<std::string>
{
public:
    using Successor = std::string;

    // A 'production_rule' is a sole symbol associated with an
    // array of symbols. In an iteration, each symbol will be
    // replaced by its associated array. Symbols without a rule
    // (terminals) are replaced by themselves.  The rules are
    // contained in a hashmap for quick access during an
    // iteration.
    using ProductionRule = RuleMap<Successor>::Rule;

    // The Rules HashMap containing the rules
    //      predecessor(char) -> successor(string):
    // Managed partially by the parent class RuleMap<> (adding and removing
    // rules, etc).
    using Rules = RuleMap<Successor>::Rules;

    // Type of the cache of all computed iterations and the axiom.
    using ProductionCache = std::unordered_map<u8, std::string>;

    // Type of the cache of all computed iteration values.
    // The second element in the pair is the maximum number of iteration for
    // this iteration.
    using IterationCache = std::unordered_map<u8, std::pair<std::vector<u8>, u8>>;

    // Constructors
    LSystem() = default;
    virtual ~LSystem() = default;
    LSystem(const std::string& axiom, const Rules& rule_map, const std::string& preds);
    LSystem(const LSystem& other) = default;
    LSystem& operator=(const LSystem& other) = default;
    LSystem(LSystem&& other) = default;
    LSystem& operator=(LSystem&& other) = default;

    // Getters and setters
    const std::string& get_axiom() const;
    const ProductionCache& get_production_cache() const;
    const std::string& get_iteration_predecessors() const;
    const IterationCache& get_iteration_cache() const;

    // Set the axiom to 'axiom'
    void set_axiom(const std::string& axiom);

    // Add the rule "predecessor -> successor"
    void add_rule(char predecessor, const Successor& successor) override;

    // Remove the rule associated to 'predecessor'
    // Exception:
    //   - Precondition: 'predecessor' must have an associated rule.
    void remove_rule(char predecessor) override;

    // Clear the rules
    void clear_rules() override;

    // Replace the rules by 'new_rules'
    void replace_rules(const Rules& new_rules) override;

    // Set the symbols flagging the iteration count 'iteration_predecessors_' to
    // 'predecessors'.
    void set_iteration_predecessors(const std::string& predecessors);

    // The result of the LSystem 'produce()' computation.
    // 'production' and 'iteration' are references to the cache to avoid
    // unecessary copies. Please be careful to the lifetime of the LSystem.
    struct LSystemProduction
    {
        const std::string& production; // The array of character derived from the axiom and rules.
        const std::vector<u8> iteration; // The array of the iteration number for each character is 'production'.
        u8 max_iteration; // The maximum number of iteration in 'iteration'
    };

    // Return a 'LSystemProduction' containing the results of the computation of
    // the 'n'-th iteration of the derivation of the axiom, rules and
    // 'iteration_predecessors_'.
    // 'size' is an optional argument that reserves 'size' elements in the
    // result vectors before starting computation. If the value is equals or
    // larger than the size of the vectors, no reallocation will take place,
    // reducing the time this function takes.
    //
    // Exceptions:
    //   - Precondition: n positive.
    //   - Ensures coherence of 'production_rules
    //   - Throw in case of allocation problem.
    //   - Throw at '.at()' if code is badly refactored.
    LSystemProduction produce(u8 n, unsigned long long size=0);

private:

    // The predecessors indicating than, at their next derivation, the iteration
    // counter will be incremented by one.
    std::string iteration_predecessors_ = {};

    // The cache of all computed iterations and the axiom.  It contains all the
    // iterations up to the highest iteration calculated. It is clearly not
    // optimized for memory usage. However, this project emphasizes
    // interactivity so quickly swapping between different iterations of the
    // same L-System is a must.
    ProductionCache production_cache_ = {};

    // The cache of all computed iteration values.
    IterationCache iteration_count_cache_ = {};


    friend class cereal::access;

    template <class Archive>
    void save (Archive& ar, const u32) const
        {
            ar(cereal::make_nvp("axiom", production_cache_.at(0)),
               cereal::make_nvp("production_rules", rules_),
               cereal::make_nvp("iteration_predecessors", iteration_predecessors_));
        }

    template <class Archive>
    void load (Archive& ar, const u32)
        {
            ar(cereal::make_nvp("axiom", production_cache_[0]),
               cereal::make_nvp("production_rules", rules_),
               cereal::make_nvp("iteration_predecessors", iteration_predecessors_));
            iteration_count_cache_[0] = {std::vector<u8>(production_cache_.at(0).size(), 0), 0};
        }
};

#endif
