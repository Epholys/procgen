#include "gsl/gsl"
#include "LSystem.h"

namespace lsys
{

    LSystem::LSystem(const std::vector<char>& ax, const production_rules& prod)
        : axiom{ax},
          rules{prod},
          result{ax}
{
}
    LSystem::LSystem(const std::string& ax, const pretty_production_rules& prod)
        : axiom{string_to_vec(ax)},
          rules{},
          result{axiom}
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
        return result;
    }

    // Exceptions:
    //   - May throw in case of allocation problem.
    //   - Precondition: n positive. Will throw otherwise.
    std::vector<char> LSystem::produce(int n)
    {
        Expects(n >= 0);

        // Reinitialize the result to the axiom.
        result = axiom;
        
        // We use a temporary vector: we can't iterate "in place".
        std::vector<char> tmp;
        
        for (int i=0; i<n; ++i) {
            tmp.clear();
            
            for (auto c : result) {
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

            result = tmp;
        }

        return result;
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

