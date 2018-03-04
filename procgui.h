#ifndef STATIC_GUI_H
#define STATIC_GUI_H


#include <sstream>

#include "imgui/imgui.h"
#include "imgui/imgui-SFML.h"
#include "LSystem.h"
#include "LSystemBuffer.h"
#include "InterpretationMapBuffer.h"
#include "Turtle.h"
#include "helper_string.h"


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
// will be in the same window.
//
// The 'main' parameter is used for a cleaner visualization when chaining
// 'display()'. When 'main' has the value 'false', the data will be displayed in
// the same window as a collapsed ImGui::TreeNode. As such, composition
// relations between objects are shown more clearly.
//
// The 'interact_with()' functions return 'true' if the object was modified by
// the GUI, 'false' otherwise.

namespace procgui {
    void display(const drawing::DrawingParameters& turtle,
                 const std::string& name,
                 bool main=true);
    
    void display(const LSystem& lsys, const std::string& name, bool main=true);
    
    void display(const drawing::InterpretationMap& map, const std::string& name, bool main=true);

    
    bool interact_with(drawing::DrawingParameters& turtle,
                       const std::string& name,
                       bool main=true);

    bool interact_with(LSystemBuffer& buffer, const std::string& name, bool main=true);

    bool interact_with(InterpretationMapBuffer& buffer, const std::string& name, bool main=true);
}

#endif
