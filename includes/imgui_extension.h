#ifndef IMGUI_EXTENSION_H
#define IMGUI_EXTENSION_H

#include "imgui/imgui.h"
#include "helper_math.h"

namespace ext::ImGui
{
    // Taken from 'imgui_demo.cpp', helper function to display a help tooltip.
    void ShowHelpMarker(const char* desc);

    // Taken from 'imgui_demo.cpp': the styles for colored buttons with a style
    // close to the default button.
    // WARNING: Use these with 'ImGui::PopStyleColor(3)'.
    template<int hue> void PushStyleColoredButton()
    {
        ::ImGui::PushStyleColor(::ImGuiCol_Button, static_cast<ImVec4>(ImColor::HSV(hue/7.0f, 0.6f, 0.6f)));
        ::ImGui::PushStyleColor(::ImGuiCol_ButtonHovered, static_cast<ImVec4>(ImColor::HSV(hue/7.0f, 0.7f, 0.7f)));
        ::ImGui::PushStyleColor(::ImGuiCol_ButtonActive, static_cast<ImVec4>(ImColor::HSV(hue/7.0f, 0.8f, 0.8f)));
    }
    enum ButtonColor
    {
        Red, Yellow, Green, Turquoise, Blue, Purple, Pink
    };

    bool DragDouble(const char* label, double* v, double v_speed = 1.0, double v_min = 0.0, double v_max = math::double_max_limit, const char* format = "%.3f", double power = 1.0);
    
    bool DragDouble2(const char* label, double v[2], double v_speed = 1.0, double v_min = 0.0, double v_max = math::double_max_limit, const char* format = "%.3f", double power = 1.0);
}

#endif // IMGUI_EXTENSION_H
