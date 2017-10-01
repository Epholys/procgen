#include "gsl/gsl"
#include "LSystem.h"

namespace lsys
{

    LSystem::LSystem(const std::vector<char>& ax, const production_rules& prod)
        : axiom{ax},
          rules{prod},
          cache{ {0, {axiom}} }
{
}
    LSystem::LSystem(const std::string& ax, const pretty_production_rules& prod)
        : axiom{string_to_vec(ax)},
          rules{},
          cache{ {0, {axiom}} }
{
    for (const auto& rule: prod) {
        rules[rule.first] = string_to_vec(rule.second);
    }
}

    std::vector<char> LSystem::get_axiom() const
    {
        return axiom;
    }

    LSystem::production_rules LSystem::get_rules() const
    {
        return rules;
    }

    std::unordered_map<int, std::vector<char>>
        LSystem::get_cache() const

    {
        return cache;
    }

    // Exceptions:
    //   - May throw in case of allocation problem.
    //   - Precondition: n positive. Will throw otherwise.
    std::vector<char> LSystem::produce(int n)
    {
        Expects(n >= 0);

        if (cache.count(n) > 0)
        {
            // A solution was already computed.
            return cache.at(n);
        }

        // The cache saves all the iteration from the start. So we get
        // the highest-iteration result.
        auto it = std::max_element(cache.begin(),
                                   cache.end(),
                                   [](const auto& pair1, const auto& pair2)
                                   { return pair1.first < pair2.first; });

        // We will start iterating from this result.
        std::vector<char> base = it->second;
        // We use a temporary vector: we can't iterate "in place".
        std::vector<char> tmp;

        int n_iter = n - it->first;
        for (int i=0; i<n_iter; ++i) {
            tmp.clear();
            
            for (auto c : base) {
                if(rules.count(c) > 0) {
                    std::vector<char> rule = rules.at(c);

                    // Replace the symbol according to its rule.
                    tmp.insert(tmp.end(), rule.begin(), rule.end());

                }
                else {
                    // The symbol is a terminal: replace it by itself.
                    tmp.push_back(c);
                }
            }

            base = tmp;
        }

        cache.emplace(n, base);
        return cache.at(n);
    }

    std::vector<char> string_to_vec (const std::string& str)
    {
        return std::vector<char> (str.begin(), str.end());
    }

    std::string vec_to_string (const std::vector<char> vec)
    {
        return std::string (vec.begin(), vec.end());
    }

}

std::ostream& operator<< (std::ostream& stream, std::vector<char> vec)
{
    std::string str (vec.begin(), vec.end());
    stream << str;
    return stream;
}

