#ifndef IMGUI_EXTENSION_H
#define IMGUI_EXTENSION_H

#include "helper_math.h"
#include "imgui/imgui.h"

namespace ext::ImGui
{
// Taken from 'imgui_demo.cpp', helper function to display a help tooltip.
void ShowHelpMarker(const char* desc);

// Taken from 'imgui_demo.cpp': the styles for colored buttons with a style
// close to the default button.
// WARNING: Use these with 'ImGui::PopStyleColor(3)'.
template<int hue>
void PushStyleColoredButton()
{
    ::ImGui::PushStyleColor(::ImGuiCol_Button,
                            static_cast<ImVec4>(ImColor::HSV(hue / 7.0f, 0.6f, 0.6f)));
    ::ImGui::PushStyleColor(::ImGuiCol_ButtonHovered,
                            static_cast<ImVec4>(ImColor::HSV(hue / 7.0f, 0.7f, 0.7f)));
    ::ImGui::PushStyleColor(::ImGuiCol_ButtonActive,
                            static_cast<ImVec4>(ImColor::HSV(hue / 7.0f, 0.8f, 0.8f)));
}
enum ButtonColor
{
    Red,
    Yellow,
    Green,
    Turquoise,
    Blue,
    Purple,
    Pink
};

bool DragDouble(const char* label,
                double* v,
                double v_speed = 1.0,
                double v_min = 0.0,
                double v_max = math::double_max_limit,
                const char* format = "%.3f",
                double power = 1.0);

bool DragDouble2(const char* label,
                 double v[2],
                 double v_speed = 1.0,
                 double v_min = 0.0,
                 double v_max = math::double_max_limit,
                 const char* format = "%.3f",
                 double power = 1.0);

bool SliderDouble(const char* label,
                  double* v,
                  double v_min,
                  double v_max,
                  const char* format = "%.3f",
                  double power = 1.0f);

bool SliderAngleDouble(const char* label,
                       double* v_rad,
                       double v_degrees_min = -360.0f,
                       double v_degrees_max = +360.0f,
                       const char* format = "%.0f deg");

// Proxy function to InputScalar like existing InputInt, InputFloat, etc.
bool InputUnsignedLongLong(const char* label,
                           unsigned long long* v,
                           unsigned long long step = 1,
                           unsigned long long step_fast = 100,
                           ImGuiInputTextFlags flags = 0);
} // namespace ext::ImGui

#endif // IMGUI_EXTENSION_H
