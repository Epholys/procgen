//  TODO Rename to buffer 
#ifndef INTERPRETATION_MAP_VIEW_H
#define INTERPRETATION_MAP_VIEW_H


#include <list>
#include <tuple>
#include <array>

#include "Turtle.h"

namespace procgui
{
    // TODO: change comments
    // // The interface between the pure logic of a LSystem and the ImGui GUI.
    // //
    // // ImGui is a immediate-mode GUI, so it can not retain information between
    // // frame. This class hosts a production rules buffer used in
    // // 'procgui::interact_with()' to manage:
    // //   - duplicate rules
    // //   - adding new rules
    // //   - removing rules
    // // We can still directly access the LSystem for all trivial attributes like
    // // the axiom.
    // //
    // // A LSystemView must be destructed before the destruction of a LSystem (non
    // // owning-reference). For the coherence of the GUI, a LSystem must have a
    // // unique LSystemView associated.
    struct InterpretationMapView
    {
        using validity    = bool; // If the rule is a duplicate, it is not valid.
        using predecessor = std::array<char, 2>;

        InterpretationMapView(drawing::interpretation_map& map);

        // Synchronize the rule buffer with the LSystem.
        void sync();

        drawing::interpretation_map& map_; // non-owning reference
        std::list<std::tuple<validity, predecessor, drawing::Order>> interpretation_buffer_;
    };
}

#endif // INTERPRETATION_MAP_VIEW_H
