#include <algorithm>
#include <gsl/gsl>
#include "ColorsGenerator.h"

namespace colors
{
    ConstantColor::ConstantColor()
        : ConstantColor(sf::Color::White)
    {
    }

    ConstantColor::ConstantColor(const sf::Color& color)
        : ColorGenerator()
        , color_{color}
    {
    }

    sf::Color ConstantColor::get(float)
    {
        return color_;
    }

    LinearGradient::LinearGradient()
        : LinearGradient({{sf::Color::Red, 0.},{sf::Color::Blue, 1.}})
    {
    }
    
    LinearGradient::LinearGradient(const LinearGradient::keys& key_colors)
        : ColorGenerator()
        , key_colors_(key_colors)
    {
        Expects(key_colors_.size() >= 2);
    }

    LinearGradient::keys LinearGradient::sanitize_keys(const keys& colors)
    {
        auto keys = colors;

        // Clamp every keys between 0 and 1.
        for (auto& p : keys)
        {
            p.second = p.second < 0. ? 0. : p.second;
            p.second = p.second > 1. ? 1. : p.second;
        }

        // Sort the elements
        std::sort(begin(keys), end(keys),
                  [](const auto& p1, const auto& p2){return p1.second < p2.second;});

        // The highest key is at 0 and the lowest at 0.
        keys.front().second = 0.f;
        keys.back().second = 1.f;

        return keys;
    }

    const LinearGradient::keys& LinearGradient::get_keys() const
    {
        return key_colors_;
    }

    void LinearGradient::set_keys(const keys& keys)
    {
        Expects(keys.size() >= 2);
        key_colors_ = keys;
    }

    sf::Color LinearGradient::get(float f)
    {
        // Clamp 'f'.
        f = f < 0. ? 0. : f;
        f = f > 1. ? 1. : f;
        auto keys = sanitize_keys(key_colors_);

        // Find the upper-bound key...
        auto superior_it = std::find_if(begin(keys), end(keys),
                                [f](const auto& p){return f <= p.second;});
        Expects(superior_it != end(keys)); // should never happen
        auto superior_idx = std::distance(begin(keys), superior_it);
        auto inferior_idx = superior_idx == 0 ? 0 : superior_idx-1; // ...and the lower-bound one.
        const auto& superior = keys.at(superior_idx);
        const auto& inferior = keys.at(inferior_idx);

        float factor = 0.f;
        if (superior == inferior)
        {
            factor = 1.f; // at the corner case f = 0
        }
        else
        {
            factor = (f - inferior.second) / (superior.second - inferior.second);
        }

        // Interpolate
        sf::Color color;
        color.r = inferior.first.r * (1-factor) + superior.first.r * factor;
        color.g = inferior.first.g * (1-factor) + superior.first.g * factor;
        color.b = inferior.first.b * (1-factor) + superior.first.b * factor;
        return color;
    }
}
