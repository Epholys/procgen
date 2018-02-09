#include "LSystemBuffer.h"

namespace procgui
{
    LSystemBuffer::LSystemBuffer(LSystem& lsys)
        : lsys_ {lsys}
        , rule_buffer_ {}
    {
        // Initialize the buffer with the LSystem's rules.
        for (const auto& rule : lsys_.get_rules())
        {
            predecessor pred = { rule.first, '\0' };
            successor   succ = string_to_array<lsys_successor_size>(rule.second);
            rule_buffer_.push_back({true, pred, succ});
        }
    }

    void LSystemBuffer::sync()
    {
        lsys_.clear_rules();

        for (const auto& rule : rule_buffer_)
        {
            char pred = std::get<predecessor>(rule).at(0);
            if(std::get<validity>(rule) &&
               pred != '\0') // An empty rule is not synchronized.
            {
                const auto& arr = std::get<successor>(rule);
                auto succ = array_to_string(arr);
                lsys_.add_rule(pred, succ);
            }
        }
    }
}
