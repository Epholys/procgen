#include "gsl/gsl"
#include "LSystem.h"


// Some functions returns references to axiom or production that could be empty.
// In these cases, it returns this string.
const static std::string empty_string = "";

LSystem::LSystem(const std::string& axiom, const Rules& prod, const std::string& preds)
    : RuleMap<std::string>(prod)
    , iteration_predecessors_ {preds}
    , production_cache_{ {0, axiom} }
    , iteration_count_cache_ { {0, {std::vector<u8>(axiom.size(), 0), 0} } }
    {
    }

const std::string& LSystem::get_axiom() const
{
    // If an axiom is defined, returns it.
    if (production_cache_.count(0) > 0)
    {
        return production_cache_.at(0);
    }
    else
    {
        return empty_string;
    }
}

const LSystem::ProductionCache& LSystem::get_production_cache() const
{
    return production_cache_;
}

const std::string& LSystem::get_iteration_predecessors() const
{
    return iteration_predecessors_;
}


const LSystem::IterationCache& LSystem::get_iteration_cache() const
{
    return iteration_count_cache_;
}

void LSystem::set_axiom(const std::string& axiom)
{
    production_cache_ = { {0, axiom} };
    iteration_count_cache_ = { {0, {std::vector<u8>(axiom.size(), 0), 0} } };
    indicate_modification();
}

void LSystem::add_rule(char predecessor, const Successor& successor)
{
    production_cache_ = { {0, get_axiom()} };
    iteration_count_cache_ = { {0, {std::vector<u8>(get_axiom().size(), 0), 0} } };
    RuleMap::add_rule(predecessor, successor);
}

void LSystem::remove_rule(char predecessor)
{
    production_cache_ = { {0, get_axiom()} };
    iteration_count_cache_ = { {0, {std::vector<u8>(get_axiom().size(), 0), 0} } };
    RuleMap::remove_rule(predecessor);
}

void LSystem::clear_rules()
{
    production_cache_ = { {0, get_axiom()} };
    iteration_count_cache_ = { {0, {{std::vector<u8>(get_axiom().size(), 0)}, 0}}};
    RuleMap::clear_rules();
}

void LSystem::replace_rules(const Rules& new_rules)
{
    production_cache_ = { {0, get_axiom()} };
    iteration_count_cache_ = { {0, {{std::vector<u8>(get_axiom().size(), 0)}, 0}}};
    RuleMap::replace_rules(new_rules);
}

void LSystem::set_iteration_predecessors(const std::string& predecessors)
{
    iteration_count_cache_ = { {0, {std::vector<u8>(get_axiom().size(), 0), 0} } };
    iteration_predecessors_ = predecessors;
    indicate_modification();
}


// Edge Cases:
//   - If 'production_cache_' is empty so does not contains the axiom, simply
//   returns an empty string.
//   - If the axiom is an empty string, early-out.
LSystem::LSystemProduction LSystem::produce(u8 n, unsigned long long size)
{
    Expects(n >= 0);

    if (production_cache_.count(0) == 0 || production_cache_.count(0) == 0)
    {
        // We do not have any axiom so nothing to produce.
        Expects(production_cache_.count(0) == production_cache_.count(0));
        return {empty_string, {}, 0};
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
    auto highest_iteration = std::max_element(iteration_count_cache_.begin(),
                                              iteration_count_cache_.end(),
                                              [](const auto& pair1, const auto& pair2)
                                              { return pair1.first < pair2.first; });

    // Invariant check: the production cache element count must be equal or
    // greater than the iteration one.
    Expects(highest_production->first >= highest_iteration->first);

    // 'iteration_predecessors_' is conveniently a std::string. But checking if
    // a predecessor is inside this string is done every time in the loop. To
    // have a little bit more performance, a map is created to have O(1) acess
    // to this information.
    std::unordered_map<char, bool> is_iteration_pred;
    for (char c : iteration_predecessors_)
    {
        is_iteration_pred[c] = true;
    }

    int max_iteration = highest_iteration->second.second;
    u8 n_iter = n - highest_iteration->first;
    for (u8 i=0; i<n_iter; ++i) {
        // We start iterating from the iteration's highest iteration.
        const std::string& base_production = production_cache_.at(highest_iteration->first + i);
        const auto& [base_iteration, _] = iteration_count_cache_.at(highest_iteration->first + i);

        // We use temporary results: we can't iterate "in place".
        std::string tmp_production;
        std::vector<u8> tmp_iteration;
        tmp_production.reserve(size);
        tmp_iteration.reserve(size);

        // If 'true', computes only the iteration vector and not the resulting
        // production string.
        bool only_iteration = highest_iteration->first + i + 1 < highest_production->first;

        // If during the derivation a rule with a 'iteration_predecessors_' is used,
        // new iteration is set to true
        bool is_new_iteration = false;


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

            // If the current predecessor must be counted, add 1 to each element
            // of the successor.
            u8 order = base_iteration.at(j);
            if (is_iteration_pred[c])
            {
                order += 1;
                is_new_iteration = true;
            }
            tmp_iteration.insert(end(tmp_iteration), successor_count, order);
        }

        if(!only_iteration)
        {
            production_cache_.try_emplace(highest_iteration->first + i + 1,
                                          std::move(tmp_production));
        }

        iteration_count_cache_.try_emplace(highest_iteration->first + i + 1,
                                           std::move(tmp_iteration),
                                           is_new_iteration ? max_iteration+1 : max_iteration);
        max_iteration = iteration_count_cache_.at(highest_iteration->first + i + 1).second;
    }

    // No 'indicate_modification()' call: this function is generally called each
    // time there is a notification of the LSystem. A second notify would double
    // the computation time and may double the computation time of the hungrier
    // 'drawing::compute_vertices()' function.

    // Ensures invariant.
    Ensures(production_cache_.size() >= iteration_count_cache_.size());

    LSystemProduction production {production_cache_.at(n),
                                  iteration_count_cache_.at(n).first,
                                  iteration_count_cache_.at(n).second};
    return production;
}
