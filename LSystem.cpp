#include "LSystem.h"

LSystem::LSystem(const std::string& ax, const production_rules& prod)
    : axiom{ax},
      rules{prod},
      current{ax}
{
}

void LSystem::reset()
{
    current = axiom;
}

std::string LSystem::get_result() const
{
    return current;
}

std::string LSystem::iter(unsigned int n_iter)
{
    // Use a temporary string: we can't iterate "in place".  
    std::string result = current;
    std::string tmp;
        
    for (unsigned int i=0; i<n_iter; ++i) {
        tmp.clear();
            
        for (auto c : result) {
            if(rules.count(c) > 0) {
                tmp.append(rules.at(c)); // Replace the symbol
                                         // according to its rule.
            }
            else {
                tmp.push_back(c);        // The symbol is a terminal:
                                         // replace it by itself.
            }
        }

        result = tmp;
    }

    current = result;
    return current;
}

std::string LSystem::get_axiom() const
{
    return axiom;
}
void LSystem::set_axiom(const std::string& str)
{
    reset(); // The current result must be coherent with the axiom. 
    axiom = str;
}

LSystem::production_rules LSystem::get_rules() const
{
    return rules;
}
void LSystem::set_rules(const production_rules& prod)
{
    reset(); // The current result must be coherent with the rules. 
    rules = prod;
}
