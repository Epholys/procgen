#ifndef GUI_H
#define GUI_H


#include <sstream>

#include "imgui/imgui.h"
#include "imgui/imgui-SFML.h"

#include "LSystem.h"
#include "Turtle.h"
#include "helper_math.h"

// void show_data(const TYPE& NAME, bool main=false);
void show_data(const lsys::LSystem& lsys, bool main=false);
void show_data(const logo::Turtle& turtle, bool main=false);


#endif
