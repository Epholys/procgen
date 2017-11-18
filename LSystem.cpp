#include "gsl/gsl"
#include "LSystem.h"


namespace lsys
{

    LSystem::LSystem(const std::vector<char>& ax, const production_rules& prod)
        : rules_{prod},
          cache_{ {0, {ax}} }
{
}
    LSystem::LSystem(const std::string& ax, const pretty_production_rules& prod)
        : rules_{},
          cache_{ {0, {string_to_vec(ax)}} }
{
    for (const auto& rule: prod) {
        rules_[rule.first] = string_to_vec(rule.second);
    }
}

    std::vector<char> LSystem::get_axiom() const
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

    std::unordered_map<int, std::vector<char>>
        LSystem::get_cache() const

    {
        return cache_;
    }

    void LSystem::set_axiom(const std::vector<char>& axiom)
    {
        cache_ = { {0, axiom} };
    }

    // Exceptions:
    //   - Precondition: n positive.
    //   - Precondition: 'cache_' is not empty and contains the axiom
    //   - Throw in case of allocation problem.
    //   - Throw at '.at()' if code is badly refactored.
    std::vector<char> LSystem::produce(int n)
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
        auto it = std::max_element(cache_.begin(),
                                   cache_.end(),
                                   [](const auto& pair1, const auto& pair2)
                                   { return pair1.first < pair2.first; });
        Expects(it != cache_.end());

        // We will start iterating from this result.
        std::vector<char> base = it->second;
        // We use a temporary vector: we can't iterate "in place".
        std::vector<char> tmp;

        int n_iter = n - it->first;
        for (int i=0; i<n_iter; ++i) {
            tmp.clear();
            
            for (auto c : base)
            {
                if(rules_.count(c) > 0)
                {
                    std::vector<char> rule = rules_.at(c);

                    // Replace the symbol according to its rule.
                    tmp.insert(tmp.end(), rule.begin(), rule.end());

                }
                else
                {
                    // The symbol is a terminal: replace it by itself.
                    tmp.push_back(c);
                }
            }

            base = tmp;
            cache_.emplace(it->first + i + 1, tmp);
        }

        return cache_.at(n);
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

