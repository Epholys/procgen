#include <algorithm>
#include <gsl/gsl>
#include "ColorsGenerator.h"

namespace colors
{
    ConstantColor::ConstantColor(const sf::Color& color)
        : ColorGenerator()
        , color_{color}
    {
    }

    sf::Color ConstantColor::get(float)
    {
        return color_;
    }
    
    
    LinearGradient::LinearGradient(const LinearGradient::keys& key_colors)
        : ColorGenerator()
        , key_colors_(key_colors)
    {
        Expects(key_colors_.size() >= 2);
        
        for (auto& p : key_colors_)
        {
            p.second = p.second < 0. ? 0. : p.second;
            p.second = p.second > 1. ? 1. : p.second;
        }

        std::sort(begin(key_colors_), end(key_colors_),
                  [](const auto& p1, const auto& p2){return p1.second < p2.second;});
        
        key_colors_.front().second = 0.f;
        key_colors_.back().second = 1.f;
    }

    sf::Color LinearGradient::get(float f)
    {
        f = f < 0. ? 0. : f;
        f = f > 1. ? 1. : f;

        auto superior_it = std::find_if(begin(key_colors_), end(key_colors_),
                                [f](const auto& p){return f <= p.second;});
        Expects(superior_it != end(key_colors_)); // should never happen
        auto superior_idx = std::distance(begin(key_colors_), superior_it);
        auto inferior_idx = superior_idx == 0 ? 0 : superior_idx-1;
        const auto& superior = key_colors_.at(superior_idx);
        const auto& inferior = key_colors_.at(inferior_idx);
        float factor = (f - inferior.second) / (superior.second - inferior.second);

        sf::Color color;
        color.r = inferior.first.r * (1-factor) + superior.first.r * factor;
        color.g = inferior.first.g * (1-factor) + superior.first.g * factor;
        color.b = inferior.first.b * (1-factor) + superior.first.b * factor;
        return color;
    }
}
