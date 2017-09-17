#include "gsl/gsl"
#include "LSystem.h"

namespace lsys
{

    LSystem::LSystem(const std::vector<char>& ax, const production_rules& prod)
        : axiom{ax},
          rules{prod},
          current{ax}
{
}
    LSystem::LSystem(const std::string& ax, const pretty_production_rules& prod)
        : axiom{string_to_vec(ax)},
          rules{},
          current{axiom}
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

    std::vector<char> LSystem::get_result() const
    {
        return current;
    }

    // Exceptions:
    //   - May throw in case of allocation problem.
    //   - Precondition: n_iter positive. Will throw otherwise.
    std::vector<char> LSystem::iter(int n_iter)
    {
        Expects(n_iter >= 0);
        
        // We use a temporary vector: we can't iterate "in place".
        std::vector<char> tmp;
        
        for (int i=0; i<n_iter; ++i) {
            tmp.clear();
            
            for (auto c : current) {
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

            current = tmp;
        }

        return current;
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

