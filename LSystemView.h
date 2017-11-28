#ifndef LSYSTEM_VIEW_H
#define LSYSTEM_VIEW_H


#include <list>
#include <tuple>
#include <array>

#include "LSystem.h"

namespace procgui
{
    // TODO: document limitation
    constexpr int lsys_input_size = 64;
    
    struct LSystemView
    {
        using predecessor = std::array<char, 2>;
        using successor   = std::array<char, lsys_input_size>;

        LSystemView(lsys::LSystem& lsys);

        void sync();

        lsys::LSystem& lsys;
        std::list<std::tuple<bool, predecessor, successor>> rule_buffer;
    };
}

#endif // LSYSTEM_VIEW_H
