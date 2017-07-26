#ifndef STATIC_GUI_H
#define STATIC_GUI_H


#include <sstream>

#include "imgui/imgui.h"
#include "imgui/imgui-SFML.h"

#include "LSystem.h"
#include "Turtle.h"
#include "helper_math.h"


// Define show_data for all printable structs and classes.
// These functions print some of the datas (attributes for example) at
// each frame to visualize important parameters and/or dynamic
// attributes.
// When calling 'show_data()', the 'main' parameters is set to true to
// define a new window. 'show_data(xxx, false)' is used internally to
// display 'xxx' inline.
// For example,
// ```
//     struct A { int n; }
//     struct B { int m;
//                A   a; }
//
//     show_data(const A& a, bool main=true) {
//         ... // display a.n
//     }
//     show_data(const B& b, bool main=true) {
//         ...                 // display b.m
//         show_data(a, false) // display a inline
//     }
// ```

void show_data(const lsys::LSystem& lsys, bool main=true);
void show_data(const logo::Turtle& turtle, bool main=true);


#endif
