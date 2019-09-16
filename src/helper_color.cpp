#include <cmath>

#include "imgui/imgui.h"
#include "helper_color.h"

namespace colors
{
    sf::Color bw_contrast_color(const sf::Color& color)
    {
        // https://gamedev.stackexchange.com/questions/38536/given-a-rgb-color-x-how-to-find-the-most-contrasting-color-y
        ImVec4 normal_color(color.r / 255.,
                            color.g / 255.,
                            color.b / 255.,
                            color.a / 255.);
        
        ImVec4 background_color_linear (std::pow(normal_color.x, 2.2),
                                        std::pow(normal_color.y, 2.2),
                                        std::pow(normal_color.z, 2.2),
                                        1.);
        float luminance = 0.2126 * background_color_linear.x +
            0.7152 * background_color_linear.y +
            0.0722 * background_color_linear.z;
            
        return luminance < 0.5 ? sf::Color::White : sf::Color::Black;
    }
}
