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

    const sf::Color& ConstantColor::get_color() const
    {
        return color_;
    }
    
    void ConstantColor::set_color(const sf::Color& color)
    {
        color_ = color;
        notify();
    }

    // Return a copy of this as a shared_ptr for polymorphic purpose.
    std::shared_ptr<ColorGenerator> ConstantColor::clone() const
    {
        return std::make_shared<ConstantColor>(*this);
    }

    //------------------------------------------------------------
    
    LinearGradient::LinearGradient()
        : LinearGradient({{sf::Color::White, 0.},{sf::Color::White, 1.}})
    {
    }

    LinearGradient::LinearGradient(const LinearGradient::keys& keys)
        : ColorGenerator()
        , keys_()
    {
        // Sanitize
        set_keys(keys);
    }
 
    const LinearGradient::keys& LinearGradient::get_keys() const
    {
        return keys_;
    }

    void LinearGradient::set_keys(const keys& keys)
    {
        Expects(keys.size() >= 2);

        keys_ = keys;
        // Clamp every keys between 0 and 1.
        for (auto& p : keys_)
        {
            p.position = p.position < 0. ? 0. : p.position;
            p.position = p.position > 1. ? 1. : p.position;
        }

        // Sort the elements.
        std::sort(begin(keys_), end(keys_),
                  [](const auto& p1, const auto& p2){return p1.position < p2.position;});
        
        // The highest key is at 1 and the lowest at 0.
        keys_.front().position = 0.f;
        keys_.back().position = 1.f;

        
        notify();
    }

    sf::Color LinearGradient::get(float f)
    {
        // Clamp 'f'.
        f = f < 0. ? 0. : f;
        f = f > 1. ? 1. : f;

        // Find the upper-bound key...
        auto superior_it = std::find_if(begin(keys_), end(keys_),
                                [f](const auto& p){return f <= p.position;});
        Expects(superior_it != end(keys_)); // (should never happen if correctly sanitized)
        auto superior_index = std::distance(begin(keys_), superior_it);
        auto inferior_index = superior_index == 0 ? 0 : superior_index-1; // ...and the lower-bound one.
        const auto& superior = keys_.at(superior_index);
        const auto& inferior = keys_.at(inferior_index);

        float factor = 0.f;
        if (superior_index == inferior_index)
        {
            factor = 1.f; // at the corner case f = 0
        }
        else
        {
            factor = (f - inferior.position) / (superior.position - inferior.position);
        }

        // Interpolate.
        sf::Color color;
        color.r = inferior.color.r * (1-factor) + superior.color.r * factor;
        color.g = inferior.color.g * (1-factor) + superior.color.g * factor;
        color.b = inferior.color.b * (1-factor) + superior.color.b * factor;
        color.a = inferior.color.a * (1-factor) + superior.color.a * factor;
        return color;
    }

    // Return a copy of this as a shared_ptr for polymorphic purpose.
    std::shared_ptr<ColorGenerator> LinearGradient::clone() const
    {
        return std::make_shared<LinearGradient>(*this);
    }

    //------------------------------------------------------------

    DiscreteGradient::DiscreteGradient()
        : DiscreteGradient({{sf::Color::White, 0}, {sf::Color::White, 1}})
    {
        generate_colors();
    }

    DiscreteGradient::DiscreteGradient(const keys& keys)
        : ColorGenerator()
        , keys_{keys}
        , colors_{}
    {
        // Verify the invariant.
        Expects(keys.size() > 1);
        Expects(keys.at(0).index == 0);
        Expects(std::is_sorted(begin(keys), end(keys),
                               [](const auto& a, const auto& b)
                               {return a.index < b.index;}));
        generate_colors();
    }

    sf::Color DiscreteGradient::get(float f)
    {
        f = f < 0. ? 0. : f;
        // We do not clamp to 1 as it would be a out-of-bound call. So we clamp
        // it to just before 1.
        f = f >= 1. ? 1.-std::numeric_limits<float>::epsilon() : f;
        
        return colors_.at(static_cast<size_t>(f * colors_.size()));
    }

    const DiscreteGradient::keys& DiscreteGradient::get_keys() const
    {
        return keys_;
    }

    const std::vector<sf::Color>& DiscreteGradient::get_colors() const
    {
        return colors_;
    }


    void DiscreteGradient::set_keys(keys keys)
    {
        // Verify the invariant
        Expects(keys.size() > 1);
        Expects(keys.at(0).index == 0);
        Expects(std::is_sorted(begin(keys), end(keys),
                               [](const auto& a, const auto& b)
                               {return a.index < b.index;}));
        keys_ = keys;
        generate_colors();
        notify();
    }

    void DiscreteGradient::generate_colors()
    {
        Expects(keys_.size() > 1);
        
        colors_.clear();

        auto inferior = keys_.begin();
        auto superior = ++keys_.begin();

        size_t i = 0;
        while(i <= keys_.back().index)
        {
            // Interpolate
            float factor = (static_cast<float>(i) - inferior->index) / (superior->index - inferior->index);
            sf::Color color;
            color.r = inferior->color.r * (1-factor) + superior->color.r * factor;
            color.g = inferior->color.g * (1-factor) + superior->color.g * factor;
            color.b = inferior->color.b * (1-factor) + superior->color.b * factor;
            color.a = inferior->color.a * (1-factor) + superior->color.a * factor;
            colors_.push_back(color);

            ++i;
            if (i > superior->index)
            {
                // Change the surrounding keys.
                ++inferior;
                ++superior;
            }
        }
    }

    // Return a copy of this as a shared_ptr for polymorphic purpose.
    std::shared_ptr<ColorGenerator> DiscreteGradient::clone() const
    {
        return std::make_shared<DiscreteGradient>(*this);
    }
}

#include "cereal/cereal.hpp"
#include "cereal/types/vector.hpp"
#include "cereal/archives/json.hpp"


CEREAL_REGISTER_TYPE_WITH_NAME(colors::ConstantColor, "ConstantColor");
CEREAL_REGISTER_POLYMORPHIC_RELATION(colors::ColorGenerator, colors::ConstantColor)
CEREAL_REGISTER_TYPE_WITH_NAME(colors::LinearGradient, "LinearGradient");
CEREAL_REGISTER_POLYMORPHIC_RELATION(colors::ColorGenerator, colors::LinearGradient)
CEREAL_REGISTER_TYPE_WITH_NAME(colors::DiscreteGradient, "DiscreteGradient");
CEREAL_REGISTER_POLYMORPHIC_RELATION(colors::ColorGenerator, colors::DiscreteGradient)

