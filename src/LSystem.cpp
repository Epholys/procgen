#include "gsl/gsl"
#include "LSystem.h"


LSystem::LSystem(const std::string& axiom, const production_rules& prod, const std::string& preds)
    : RuleMap<std::string>(prod)
    , recursion_predecessors_ {preds}
    , production_cache_{ {0, axiom} }
    , recursion_cache_ { {0, std::vector<int>(axiom.size(), 0)} }
    {
    }

std::string LSystem::get_axiom() const
{
    // If an axiom is defined, returns it.
    if (production_cache_.count(0) > 0)
    {
        return production_cache_.at(0);
    }
    else
    {
        return {};
    }
}

const std::unordered_map<int, std::string>& LSystem::get_production_cache() const
{
    return production_cache_;
}

std::string LSystem::get_recursion_predecessors() const
{
    return recursion_predecessors_;
}


const std::unordered_map<int, std::vector<int>>& LSystem::get_recursion_cache() const
{
    return recursion_cache_;
}

void LSystem::set_axiom(const std::string& axiom)
{
    production_cache_ = { {0, axiom} };
    recursion_cache_ = { {0, std::vector<int>(axiom.size(), 0) } };
    notify();
} 

void LSystem::add_rule(char predecessor, const RuleMap::successor& successor)
{
    production_cache_ = { {0, get_axiom()} };
    recursion_cache_ = { {0, std::vector<int>(get_axiom().size(), 0) } };
    RuleMap::add_rule(predecessor, successor);    
}

void LSystem::remove_rule(char predecessor)
{
    production_cache_ = { {0, get_axiom()} };
    recursion_cache_ = { {0, std::vector<int>(get_axiom().size(), 0) } };
    RuleMap::remove_rule(predecessor);
}

void LSystem::clear_rules()
{
    production_cache_ = { {0, get_axiom()} };
    recursion_cache_ = { {0, std::vector<int>(get_axiom().size(), 0) } };
    RuleMap::clear_rules();
}                             

void LSystem::set_recursion_predecessors(const std::string& predecessors)
{
    recursion_cache_ = { {0, std::vector<int>(get_axiom().size(), 0) } };
    recursion_predecessors_ = predecessors;
    notify();
}


// Edge Cases:
//   - If 'production_cache_' is empty so does not contains the axiom, simply
//   returns an empty string.
//   - If the axiom is an empty string, early-out.
std::pair<std::string, std::vector<int>> LSystem::produce(int n)
{
    Expects(n >= 0);

    if (production_cache_.count(0) == 0 || production_cache_.count(0) == 0)
    {
        // We do not have any axiom so nothing to produce.
        Expects(production_cache_.count(0) == production_cache_.count(0));
        return {"", {}};
    }
        
    if (production_cache_.count(n) > 0 && recursion_cache_.count(n) > 0)
    {
        // A solution was already computed.
        return {production_cache_.at(n), recursion_cache_.at(n)};
    }

    // The caches saves all the iteration from the start. So we get
    // the highest-iteration result for each cache.
    auto highest_production = std::max_element(production_cache_.begin(),
                                               production_cache_.end(),
                                               [](const auto& pair1, const auto& pair2)
                                               { return pair1.first < pair2.first; });
    auto highest_recursion= std::max_element(recursion_cache_.begin(),
                                                   recursion_cache_.end(),
                                                   [](const auto& pair1, const auto& pair2)
                                                   { return pair1.first < pair2.first; });

    // Invariant check: the production cache element count must be equal or
    // greater than the recursion one.
    Expects(highest_production->first >= highest_recursion->first);
    
    // We start iterating from the recursion's highest iteration.
    std::string base_production = production_cache_.at(highest_recursion->first);
    std::vector<int> base_recursion = highest_recursion->second;
    
    // We use temporary results: we can't iterate "in place".
    std::string tmp_production;
    std::vector<int> tmp_recursion;    

    int n_iter = n - highest_recursion->first;
    for (int i=0; i<n_iter; ++i) {
        tmp_production.clear();
        tmp_recursion.clear();

        // If 'true', computes only the recursion vector and not the resulting
        // production string.
        bool only_recursion = highest_recursion->first + i + 1 < highest_production->first;

        for (auto j=0u; j<base_recursion.size(); ++j)
        {
            char c = base_production.at(j);
            int successor_count = 0;

            if (rules_.count(c) > 0)
            {
                // Add n element to the recursion vector, n corresponding to the
                // successor size.
                successor_count = rules_.at(c).size();

                if (!only_recursion)
                {
                    std::string derivation = rules_.at(c);

                    // Replace the symbol according to its rule.
                    tmp_production.insert(tmp_production.end(), derivation.begin(), derivation.end());
                }
            }
            else // The identity rule
            {
                // Add only one element.
                successor_count = 1;
                    
                if (!only_recursion)
                {
                    // The symbol is a terminal: replace it by itself.
                    tmp_production.push_back(c);
                }
            }

            // If the current predecessor must be counter, add 1 to each element
            // of the successor.
            char order = base_recursion.at(j);
            if (recursion_predecessors_.find(c) != std::string::npos)
            {
                order += 1;
            }
            tmp_recursion.insert(end(tmp_recursion), successor_count, order);
        }

        if(only_recursion)
        {
            base_production = production_cache_.at(highest_recursion->first + i + 1);
        }
        else
        {
            base_production = tmp_production;
            production_cache_.emplace(highest_production->first + i + 1, tmp_production);
        }

        base_recursion = tmp_recursion;
        recursion_cache_.emplace(highest_recursion->first + i + 1, tmp_recursion);
    }

    // No 'notify()' call: this function is generally called each time there is
    // a notification of the LSystem. A second notify would double the
    // computation time and may double the computation time of the hungrier
    // 'drawing::compute_vertices()' function.
    
    return {production_cache_.at(n), recursion_cache_.at(n)};
}

