#include "imgui_extension.h"

namespace ext::ImGui
{
    // Taken from 'imgui_demo.cpp', helper function to display a help tooltip.
    void ShowHelpMarker(const char* desc)
    {
        ::ImGui::TextDisabled("(?)");
        if (::ImGui::IsItemHovered())
        {
            ::ImGui::BeginTooltip();
            ::ImGui::PushTextWrapPos(450.0f);
            ::ImGui::TextUnformatted(desc);
            ::ImGui::PopTextWrapPos();
            ::ImGui::EndTooltip();
        }
    }

    bool DragDouble(const char* label, double* v, double v_speed, double v_min, double v_max, const char* format, double power)
    {
        double* min = &v_min;
        double* max = &v_max;
        return ::ImGui::DragScalarN(label, ::ImGuiDataType_Double, v, 1, v_speed, min, max, format, power);
    }
    bool DragDouble2(const char* label, double v[2], double v_speed, double v_min, double v_max, const char* format, double power)
    {
        double* min = &v_min;
        double* max = &v_max;
        return ::ImGui::DragScalarN(label, ::ImGuiDataType_Double, v, 2, v_speed, min, max, format, power);
    }
}
