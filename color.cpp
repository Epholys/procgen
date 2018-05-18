#include <iostream>
#include "imgui/imgui.h"
#include "color.h"

namespace color
{
    sf::Color unique_col_100(int id)
    {
        float r, g, b;
        float h = ((id * 87) % 100) / 100.;
        ImGui::ColorConvertHSVtoRGB(h, .8, .8, r, g, b);
        return sf::Color(r*255, g*255, b*255);
    }
}
