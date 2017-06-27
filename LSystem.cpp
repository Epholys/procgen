#include "LSystem.h"


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


// void LSystem::reset()
// {
//     current = axiom;
// }


// void LSystem::set_axiom(const std::vector<char>& str)
// {
//     axiom = str;
//     reset(); // The current result must be coherent with the axiom:
//              // reset the current string to the new axiom
// }

// void LSystem::set_rules(const production_rules& prod)
// {
//     rules = prod;
//     reset(); // The current result must be coherent with the rule:
//              // reset the new string to the axiom
// }


std::vector<char> LSystem::get_result() const
{
    return current;
}

// TODO: Remove 'unsigned int', add Expects().
// Note: May throw in case of allocation problem.
std::vector<char> LSystem::iter(unsigned int n_iter)
{
    // Use a temporary vector: we can't iterate "in place".
    std::vector<char> result = current;
    std::vector<char> tmp;
        
    for (unsigned int i=0; i<n_iter; ++i) {
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

    current = result;
    return current;
}

std::ostream& operator<< (std::ostream& stream, std::vector<char> vec)
{
    std::string str (vec.begin(), vec.end());
    stream << str;
    return stream;
}

std::vector<char> string_to_vec (const std::string& str)
{
    return std::vector<char> (str.begin(), str.end());
}
