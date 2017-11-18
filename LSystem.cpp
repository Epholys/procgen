#include "gsl/gsl"
#include "LSystem.h"


namespace lsys
{
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

    LSystem::production_rules LSystem::get_rules() const
    {
        return rules_;
    }

    std::unordered_map<int, std::string>
        LSystem::get_cache() const

    {
        return cache_;
    }

    void LSystem::set_axiom(const std::string& axiom)
    {
        cache_ = { {0, axiom} };
    }

    // Exceptions:
    //   - Precondition: n positive.
    //   - Precondition: 'cache_' is not empty and contains the axiom
    //   - Throw in case of allocation problem.
    //   - Throw at '.at()' if code is badly refactored.
    std::string LSystem::produce(int n)
    {
        Expects(n >= 0);
        Expects(cache_.count(0) > 0);

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

        return cache_.at(n);
    }

}
