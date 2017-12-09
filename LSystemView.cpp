#include "LSystemView.h"

namespace procgui
{
    LSystemView::LSystemView(lsys::LSystem& lsys)
        : lsys {lsys}
        , rule_buffer {}
    {
        // TODO: but this in a helper func
        auto string_to_arr = [](auto str)
            { std::array<char, lsys_input_size> arr;
              str.resize(lsys_input_size-1, '\0');
              for(std::size_t i=0; i<str.size(); ++i)
                  arr.at(i) = str.at(i);
              arr.back() = '\0';
              return arr;
            };
            
        for (const auto& rule : lsys.get_rules())
        {
            predecessor pred = { rule.first, '\0' };
            successor succ   = string_to_arr(rule.second);
            rule_buffer.push_back({true, pred, succ});
        }
    }

    void LSystemView::sync()
    {
        lsys.clear_rules();
        
        for (const auto& rule : rule_buffer)
        {
            if(std::get<validity>(rule))
            {
                char pred = std::get<predecessor>(rule).at(0);
                const auto& arr = std::get<successor>(rule);
                std::string succ { arr.begin(),
                                   std::find(arr.begin(), arr.end(), '\0') };

                lsys.add_rule(pred, succ);
            }
        }
    }
}
