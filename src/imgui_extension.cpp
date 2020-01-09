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

bool DragDouble(const char* label,
                double* v,
                double v_speed,
                double v_min,
                double v_max,
                const char* format,
                double power)
{
    double* min = &v_min;
    double* max = &v_max;
    return ::ImGui::DragScalarN(label,
                                ::ImGuiDataType_Double,
                                v,
                                1,
                                v_speed,
                                min,
                                max,
                                format,
                                power);
}
bool DragDouble2(const char* label,
                 double v[2],
                 double v_speed,
                 double v_min,
                 double v_max,
                 const char* format,
                 double power)
{
    double* min = &v_min;
    double* max = &v_max;
    return ::ImGui::DragScalarN(label,
                                ::ImGuiDataType_Double,
                                v,
                                2,
                                v_speed,
                                min,
                                max,
                                format,
                                power);
}

bool SliderDouble(const char* label,
                  double* v,
                  double v_min,
                  double v_max,
                  const char* format,
                  double power)
{
    return ::ImGui::SliderScalar(label, ::ImGuiDataType_Double, v, &v_min, &v_max, format, power);
}

bool SliderAngleDouble(const char* label,
                       double* v_rad,
                       double v_degrees_min,
                       double v_degrees_max,
                       const char* format)
{
    if (format == NULL)
    {
        format = "%.0f deg";
    }
    double v_deg = (*v_rad) * 360.0f / (2 * math::pi);
    bool value_changed = SliderDouble(label, &v_deg, v_degrees_min, v_degrees_max, format, 1.0f);
    *v_rad = v_deg * (2 * math::pi) / 360.0f;
    return value_changed;
}

bool InputUnsignedLongLong(const char* label,
                           unsigned long long* v,
                           unsigned long long step,
                           unsigned long long step_fast,
                           ImGuiInputTextFlags flags)
{
    // Hexadecimal input provided as a convenience but the flag name is awkward. Typically you'd use
    // InputText() to parse your own data, if you want to handle prefixes.
    const char* format = (flags & ImGuiInputTextFlags_CharsHexadecimal) != 0 ? "%08X" : "%llu";
    return ::ImGui::InputScalar(label,
                                ImGuiDataType_U64,
                                (void*)v,
                                (void*)(step > 0 ? &step : NULL),
                                (void*)(step_fast > 0 ? &step_fast : NULL),
                                format,
                                flags);
}
} // namespace ext::ImGui
