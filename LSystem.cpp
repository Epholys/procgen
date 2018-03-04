#include "gsl/gsl"
#include "LSystem.h"


LSystem::LSystem(const std::string& axiom, const production_rules& prod)
    : rules_{prod},
      cache_{ {0, axiom} }
    {
    }

std::string LSystem::get_axiom() const
{
    // If an axiom is defined, returns it.
    if (cache_.count(0) > 0)
    {
        return cache_.at(0);
    }
    else
    {
        return {};
    }
}

bool LSystem::has_predecessor(char predecessor) const
{
    return rules_.count(predecessor) > 0;
}

bool LSystem::has_rule(char predecessor, const std::string& successor) const
{
    return has_predecessor(predecessor) && rules_.at(predecessor) == successor;
}

LSystem::rule LSystem::get_rule(char predecessor) const
{
    Expects(has_predecessor(predecessor));
    return { predecessor, rules_.at(predecessor) };
}

const LSystem::production_rules& LSystem::get_rules() const
{
    return rules_;
}

const std::unordered_map<int, std::string>& LSystem::get_cache() const

{
    return cache_;
}

void LSystem::set_axiom(const std::string& axiom)
{
    cache_ = { {0, axiom} };
    notify();
}

void LSystem::add_rule(char predecessor, const std::string& successor)
{
    cache_ = { {0, get_axiom()} };
    rules_[predecessor] = successor;
    notify();
}

void LSystem::remove_rule(char predecessor)
{
    auto rule = rules_.find(predecessor);
    Expects(rule != rules_.end());

    cache_ = { {0, get_axiom()} };
    rules_.erase(rule);
    notify();
}

void LSystem::clear_rules()
{
    cache_ = { {0, get_axiom()} };
    rules_.clear();
    notify();
}                             
    
// Exceptions:
//   - Precondition: n positive.
//   - Throw in case of allocation problem.
//   - Throw at '.at()' if code is badly refactored.
// Edge Cases:
//   - If 'cache_' is empty so does not contains the axiom, simply
//   returns an empty string.
//   - If the axiom is an empty string, early-out.
std::string LSystem::produce(int n)
{
    Expects(n >= 0);

    if (cache_.count(0) == 0 || cache_.at(0) == "")
    {
        // We do not have any axiom so nothing to produce.
        return "";
    }
        
    if (cache_.count(n) > 0)
    {
        // A solution was already computed.
        return cache_.at(n);
    }

    // The cache saves all the iteration from the start. So we get
    // the highest-iteration result.
    auto highest = std::max_element(cache_.begin(),
                                    cache_.end(),
                                    [](const auto& pair1, const auto& pair2)
                                    { return pair1.first < pair2.first; });

    // We will start iterating from this result.
    std::string base = highest->second;
    // We use a temporary string: we can't iterate "in place".
    std::string tmp;

    int n_iter = n - highest->first;
    for (int i=0; i<n_iter; ++i) {
        tmp.clear();
            
        for (auto c : base)
        {
            if(rules_.count(c) > 0)
            {
                std::string derivation = rules_.at(c);

                // Replace the symbol according to its rule.
                tmp.insert(tmp.end(), derivation.begin(), derivation.end());

            }
            else
            {
                // The symbol is a terminal: replace it by itself.
                tmp.push_back(c);
            }
        }

        base = tmp;
        cache_.emplace(highest->first + i + 1, tmp);
    }

    notify();

    return cache_.at(n);
}

