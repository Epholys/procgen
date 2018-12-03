#include "gsl/gsl"
#include "LSystem.h"


LSystem::LSystem(const std::string& axiom, const production_rules& prod, const std::string& preds)
    : RuleMap<std::string>(prod)
    , iteration_predecessors_ {preds}
    , production_cache_{ {0, axiom} }
    , iteration_count_cache_ { {0, {std::vector<int>(axiom.size(), 0), 0} } }
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

std::string LSystem::get_iteration_predecessors() const
{
    return iteration_predecessors_;
}


const std::unordered_map<int, std::pair<std::vector<int>, int>>& LSystem::get_iteration_cache() const
{
    return iteration_count_cache_;
}

void LSystem::set_axiom(const std::string& axiom)
{
    production_cache_ = { {0, axiom} };
    iteration_count_cache_ = { {0, {std::vector<int>(axiom.size(), 0), 0} } };
    notify();
} 

void LSystem::add_rule(char predecessor, const RuleMap::successor& successor)
{
    production_cache_ = { {0, get_axiom()} };
    iteration_count_cache_ = { {0, {std::vector<int>(get_axiom().size(), 0), 0} } };
    RuleMap::add_rule(predecessor, successor);    
}

void LSystem::remove_rule(char predecessor)
{
    production_cache_ = { {0, get_axiom()} };
    iteration_count_cache_ = { {0, {std::vector<int>(get_axiom().size(), 0), 0} } };
    RuleMap::remove_rule(predecessor);
}

void LSystem::clear_rules()
{
    production_cache_ = { {0, get_axiom()} };
    iteration_count_cache_ = { {0, {{std::vector<int>(get_axiom().size(), 0)}, 0}}};
    RuleMap::clear_rules();
}                             

void LSystem::set_iteration_predecessors(const std::string& predecessors)
{
    iteration_count_cache_ = { {0, {std::vector<int>(get_axiom().size(), 0), 0} } };
    iteration_predecessors_ = predecessors;
    notify();
}


// Edge Cases:
//   - If 'production_cache_' is empty so does not contains the axiom, simply
//   returns an empty string.
//   - If the axiom is an empty string, early-out.
std::tuple<std::string, std::vector<int>, int> LSystem::produce(int n)
{
    Expects(n >= 0);

    if (production_cache_.count(0) == 0 || production_cache_.count(0) == 0)
    {
        // We do not have any axiom so nothing to produce.
        Expects(production_cache_.count(0) == production_cache_.count(0));
        return {"", {}, 0};
    }
        
    if (production_cache_.count(n) > 0 && iteration_count_cache_.count(n) > 0)
    {
        // A solution was already computed.
        return {production_cache_.at(n),
                iteration_count_cache_.at(n).first,
                iteration_count_cache_.at(n).second};
    }

    // The caches saves all the iteration from the start. So we get
    // the highest-iteration result for each cache.
    auto highest_production = std::max_element(production_cache_.begin(),
                                               production_cache_.end(),
                                               [](const auto& pair1, const auto& pair2)
                                               { return pair1.first < pair2.first; });
    auto highest_iteration= std::max_element(iteration_count_cache_.begin(),
                                                   iteration_count_cache_.end(),
                                                   [](const auto& pair1, const auto& pair2)
                                                   { return pair1.first < pair2.first; });

    // Invariant check: the production cache element count must be equal or
    // greater than the iteration one.
    Expects(highest_production->first >= highest_iteration->first);
    
    // We start iterating from the iteration's highest iteration.
    std::string base_production = production_cache_.at(highest_iteration->first);
    auto [base_iteration, max_iteration] = highest_iteration->second;
    
    
    // We use temporary results: we can't iterate "in place".
    std::string tmp_production;
    std::vector<int> tmp_iteration;    

    int n_iter = n - highest_iteration->first;
    for (int i=0; i<n_iter; ++i) {
        tmp_production.clear();
        tmp_iteration.clear();

        // If 'true', computes only the iteration vector and not the resulting
        // production string.
        bool only_iteration = highest_iteration->first + i + 1 < highest_production->first;

        // If during the derivation a rule with a 'iteration_predecessors_' is used,
        // new iteration is set to true
        bool new_iteration = false;


        for (auto j=0u; j<base_iteration.size(); ++j)
        {
            char c = base_production.at(j);
            int successor_count = 0;

            if (rules_.count(c) > 0)
            {
                // Add n element to the iteration vector, n corresponding to the
                // successor size.
                successor_count = rules_.at(c).size();

                if (!only_iteration)
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
                    
                if (!only_iteration)
                {
                    // The symbol is a terminal: replace it by itself.
                    tmp_production.push_back(c);
                }
            }

            // If the current predecessor must be counter, add 1 to each element
            // of the successor.
            char order = base_iteration.at(j);
            if (iteration_predecessors_.find(c) != std::string::npos)
            {
                order += 1;
                new_iteration = true;
            }
            tmp_iteration.insert(end(tmp_iteration), successor_count, order);
        }

        if(only_iteration)
        {
            base_production = production_cache_.at(highest_iteration->first + i + 1);
        }
        else
        {
            base_production = tmp_production;
            production_cache_.emplace(highest_production->first + i + 1, tmp_production);
        }

        base_iteration = tmp_iteration;
        iteration_count_cache_[highest_iteration->first + i + 1] =
                                 {tmp_iteration, new_iteration ? max_iteration+1 : max_iteration};
        max_iteration = iteration_count_cache_.at(highest_iteration->first + i + 1).second;
    }

    // No 'notify()' call: this function is generally called each time there is
    // a notification of the LSystem. A second notify would double the
    // computation time and may double the computation time of the hungrier
    // 'drawing::compute_vertices()' function.
    
    return {production_cache_.at(n), iteration_count_cache_.at(n).first, iteration_count_cache_.at(n).second};
}

