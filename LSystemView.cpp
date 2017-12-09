#include "LSystemView.h"
#include "helper_string.h"

namespace procgui
{
    LSystemView::LSystemView(lsys::LSystem& lsys)
        : lsys {lsys}
        , rule_buffer {}
    {
        for (const auto& rule : lsys.get_rules())
        {
            predecessor pred = { rule.first, '\0' };
            successor   succ = string_to_array<lsys_input_size>(rule.second);
            rule_buffer.push_back({true, pred, succ});
        }
    }

    void LSystemView::sync()
    {
        lsys.clear_rules();

        for (const auto& rule : rule_buffer)
        {
            char pred = std::get<predecessor>(rule).at(0);
            if(std::get<validity>(rule) && pred != '\0')
            {
                const auto& arr = std::get<successor>(rule);
                auto succ = array_to_string(arr);
                lsys.add_rule(pred, succ);
            }
        }
    }
}
