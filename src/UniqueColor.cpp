#include "UniqueColor.h"

#include "imgui/imgui.h"

#include <cmath>
#include <gsl/gsl>

namespace colors
{
sf::Color UniqueColor::get_color(int id)
{
    Expects(id >= 0 && id <= long(colors_.size()));
    if (id == long(colors_.size()))
    {
        return new_color();
    }
    else
    {
        return colors_.at(id);
    }
}

// https://blog.bruce-hill.com/6-useful-snippets
sf::Color UniqueColor::new_color()
{
    static const float golden_ratio = (std::sqrt(5) + 1) / 2.;

    ++iter_;
    float hue = std::fmod(iter_ * golden_ratio, 1.);

    float r, g, b;
    ImGui::ColorConvertHSVtoRGB(hue, .8, .8, r, g, b);
    sf::Color new_color(r * 255, g * 255, b * 255);
    colors_.push_back(new_color);
    return new_color;
}
} // namespace colors
