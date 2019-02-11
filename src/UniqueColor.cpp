#include <iostream>
#include <cmath>
#include <gsl/gsl>
#include "imgui/imgui.h"
#include "UniqueColor.h"

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

    sf::Color UniqueColor::new_color()
    {
        float hue = 0.;

        // For the first pass, subdivide the [0,1] range in 5 values.
        if (pass_ == 0)
        {
            hue = 20.*iter_ / 100.;
            ++iter_;
            if (iter_ == 5)
            {
                ++pass_;
                iter_ = 0;
            }
        }
        // For the other pass, 'hue' is the middle value between two
        // subdivisions of the previous pass.
        else
        {
            hue = (20./std::pow(2, pass_)) * (2*iter_+1) / 100.;
            ++iter_;
            if (iter_ == 5 * std::pow(2, pass_-1))
            {
                ++pass_;
                iter_ = 0;
            }
        }

        float r, g, b;
        ImGui::ColorConvertHSVtoRGB(hue, .8, .8, r, g, b);
        sf::Color new_color (r*255, g*255, b*255);
        colors_.push_back(new_color);
        return  new_color;
    }
}
