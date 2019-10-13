#ifndef PROCEDURAL_GUI_H
#define PROCEDURAL_GUI_H


#include <sstream>

#include "imgui/imgui.h"
#include "imgui/imgui-SFML.h"
#include "LSystem.h"
#include "RuleMapBuffer.h"
#include "LSystemBuffer.h"
#include "LSystemView.h"
#include "InterpretationMapBuffer.h"
#include "Turtle.h"
#include "helper_string.h"
#include "ColorsGenerator.h"
#include "ColorsGeneratorWrapper.h"
#include "VertexPainterWrapper.h"

// Define 'display()' and 'interact_with()' for the structs and classes to
// display.
// 
// The 'display()' functions display some of the data (attributes for example)
// at each frame to visualize important parameters and/or dynamic attributes.
//
// The 'interact_with()' functions not only display the data, but also give the
// possibility to modify them on-the-fly, allowing a great deal of
// interactivity.
//
// The 'name' parameter will be the name of the window. If 'display()' is called
// with the same name for two different objects, even with different types, they
// will be in the same window. Exception: the LSystemView which is a coherent
// and unique window.

namespace procgui {
    
    void display(const drawing::DrawingParameters& turtle,
                 const std::string& name);
    
    void display(const LSystem& lsys, const std::string& name);
    
    void display(const drawing::InterpretationMap& map, const std::string& name);

    
    void interact_with(drawing::DrawingParameters& turtle,
                       const std::string& name);

    // Special case for the RuleMapBuffers
    template<typename Buffer>
    void interact_with_buffer(Buffer& buffer,
                              std::function<bool(typename Buffer::const_iterator)> successor_fn);
    
    void interact_with(LSystemBuffer& buffer, const std::string& name);

    void interact_with(InterpretationMapBuffer& buffer, const std::string& name);

    // 'open' manages the selection of the LSystemView: if it is true, the
    // window is keeped open. Otherwise, when clicking at the close widget, the
    // view is un-selected and the window closed.
    void interact_with(LSystemView& lsys_view,
                       const std::string& name,
                       bool is_modified,
                       bool* open = nullptr);

    void interact_with_graphics_parameters(bool& box_is_visible);
    
    void interact_with(colors::VertexPainterWrapper& painter_wrapper,
                       const std::string& name);

    enum class color_wrapper_mode
    {
        CONSTANT = 1,
        GRADIENTS = 1 << 1,
        ALL = CONSTANT | GRADIENTS,
    };
    void interact_with(colors::ColorGeneratorWrapper& color_wrapper,
                       const std::string& name,
                       color_wrapper_mode mode = color_wrapper_mode::ALL);

    #include "procgui.tpp"

}


#endif
