#ifndef LSYSTEM_VIEW_H
#define LSYSTEM_VIEW_H


#include <list>
#include <tuple>
#include <array>

#include "LSystem.h"
#include "helper_string.h"

namespace procgui
{
    // ImGui needs a number for its fixed-size input buffer.
    // We arbitrarily choose 64 characters as the limit for the sucessor of a
    // production rule.
    constexpr int lsys_successor_size = 64;

    // The interface between the pure logic of a LSystem and the ImGui GUI.
    //
    // ImGui is a immediate-mode GUI, so it can not retain information between
    // frame. This class hosts a production rules buffer used in
    // 'procgui::interact_with()' to manage:
    //   - duplicate rules
    //   - adding new rules
    //   - removing rules
    // We can still directly access the LSystem for all trivial attributes like
    // the axiom.
    //
    // A LSystemBuffer must be destructed before the destruction of its LSystem
    // (non owning-reference). For the coherence of the GUI, a LSystem must have
    // a unique LSystemBuffer associated.
    //
    // Note: This class has a lot in common with 'IntepretationMapBuffer'. If a
    // third class has the same properties, all will be refactorized.
    struct LSystemBuffer
    {
        using validity    = bool; // If the rule is a duplicate, it is not valid.
        using predecessor = std::array<char, 2>;
        using successor   = std::array<char, lsys_successor_size>;

        LSystemBuffer(LSystem& lsys);

        // Synchronize the rule buffer with the LSystem.
        void sync();

        LSystem& lsys_; // non-owning reference
        std::list<std::tuple<validity, predecessor, successor>> rule_buffer_;
    };
}

#endif // LSYSTEM_VIEW_H
