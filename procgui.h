#ifndef STATIC_GUI_H
#define STATIC_GUI_H


#include <sstream>

#include "imgui/imgui.h"
#include "imgui/imgui-SFML.h"
#include "LSystem.h"
#include "Turtle.h"


// Define 'display_data()' for the structs and classes to display.
// 
// These functions display some of the datas (attributes for example) at
// each frame to visualize important parameters and/or dynamic
// attributes.
// 
// The 'name' parameter will be the name of the window. If
// 'display_data()' is called with the same name for two different
// objects, even with different types, they will be in the same window.
//
// The 'main' parameter is used for a cleaner visualization when
// chaining 'display_data()'. When 'main' has the value 'false', the
// data will be displayed in the same window as a collapsed
// TreeNode. As such, composition relations between objects are shown
// more clearly.
// This behaviour is implemented by the 'set_up()' and 'conclude()'
// functions.

namespace procgui {
    // Open a window named 'name' if 'main' is true.
    // Otherwise, set up a TreeNode named 'name'.
    // Returns 'false' if the window is collapsed, allowing
    // 'display_data()' to early out.
    bool set_up(const std::string& name, bool main);

    // Finish appending to the current window if 'main' is true.
    // Otherwise, close the current TreeNode.
    void conclude(bool main);

    void display_data(const logo::Turtle& turtle,const std::string& name, bool main=true);
    void display_data(const lsys::LSystem& lsys, const std::string& name, bool main=true);
}

#endif
