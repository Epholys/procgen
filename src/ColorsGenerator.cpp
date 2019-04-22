#include <algorithm>
#include <gsl/gsl>
#include "ColorsGenerator.h"

namespace colors
{
    // Returns a cloned copy of this. Managed by the children.
    std::shared_ptr<ColorGenerator> ColorGenerator::clone() const
    {
        return clone_impl();
    }

    //------------------------------------------------------------
    
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
    std::shared_ptr<ColorGenerator> ConstantColor::clone_impl() const
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
        , raw_keys_(keys)
        , sanitized_keys_(keys)
    {
        Expects(keys.size() >= 2);
        sanitize_keys();
    }

    void LinearGradient::sanitize_keys()
    {
        sanitized_keys_ = raw_keys_;
        
        // Clamp every keys between 0 and 1.
        for (auto& p : sanitized_keys_)
        {
            p.second = p.second < 0. ? 0. : p.second;
            p.second = p.second > 1. ? 1. : p.second;
        }

        // Sort the elements.
        std::sort(begin(sanitized_keys_), end(sanitized_keys_),
                  [](const auto& p1, const auto& p2){return p1.second < p2.second;});

        // The highest key is at 1 and the lowest at 0.
        sanitized_keys_.front().second = 0.f;
        sanitized_keys_.back().second = 1.f;
    }

    const LinearGradient::keys& LinearGradient::get_raw_keys() const
    {
        return raw_keys_;
    }
 
    const LinearGradient::keys& LinearGradient::get_sanitized_keys() const
    {
        return sanitized_keys_;
    }

    void LinearGradient::set_keys(const keys& keys)
    {
        Expects(keys.size() >= 2);
        raw_keys_ = keys;
        sanitize_keys();
        notify();
    }

    sf::Color LinearGradient::get(float f)
    {
        // Clamp 'f'.
        f = f < 0. ? 0. : f;
        f = f > 1. ? 1. : f;

        // Find the upper-bound key...
        auto superior_it = std::find_if(begin(sanitized_keys_), end(sanitized_keys_),
                                [f](const auto& p){return f <= p.second;});
        Expects(superior_it != end(sanitized_keys_)); // (should never happen if correctly sanitized)
        auto superior_index = std::distance(begin(sanitized_keys_), superior_it);
        auto inferior_index = superior_index == 0 ? 0 : superior_index-1; // ...and the lower-bound one.
        const auto& superior = sanitized_keys_.at(superior_index);
        const auto& inferior = sanitized_keys_.at(inferior_index);

        float factor = 0.f;
        if (superior == inferior)
        {
            factor = 1.f; // at the corner case f = 0
        }
        else
        {
            factor = (f - inferior.second) / (superior.second - inferior.second);
        }

        // Interpolate.
        sf::Color color;
        color.r = inferior.first.r * (1-factor) + superior.first.r * factor;
        color.g = inferior.first.g * (1-factor) + superior.first.g * factor;
        color.b = inferior.first.b * (1-factor) + superior.first.b * factor;
        color.a = inferior.first.a * (1-factor) + superior.first.a * factor;
        return color;
    }

    // Return a copy of this as a shared_ptr for polymorphic purpose.
    std::shared_ptr<ColorGenerator> LinearGradient::clone_impl() const
    {
        return std::make_shared<LinearGradient>(*this);
    }

    //------------------------------------------------------------

    DiscreteGradient::DiscreteGradient()
        : DiscreteGradient({{sf::Color::White, 0}, {sf::Color::White, 1}})
    {
    }

    DiscreteGradient::DiscreteGradient(const keys& keys)
        : ColorGenerator()
        , keys_{keys}
        , colors_{}
    {
        // Verify the invariant.
        Expects(keys.size() > 1);
        Expects(keys.at(0).second == 0);
        Expects(std::is_sorted(begin(keys), end(keys),
                               [](const auto& a, const auto& b)
                               {return a.second < b.second;}));
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
        Expects(keys.at(0).second == 0);
        Expects(std::is_sorted(begin(keys), end(keys),
                               [](const auto& a, const auto& b)
                               {return a.second < b.second;}));
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
        while(i <= keys_.back().second)
        {
            // Interpolate
            float factor = (static_cast<float>(i) - inferior->second) / (superior->second - inferior->second);
            sf::Color color;
            color.r = inferior->first.r * (1-factor) + superior->first.r * factor;
            color.g = inferior->first.g * (1-factor) + superior->first.g * factor;
            color.b = inferior->first.b * (1-factor) + superior->first.b * factor;
            color.a = inferior->first.a * (1-factor) + superior->first.a * factor;
            colors_.push_back(color);

            ++i;
            if (i > superior->second)
            {
                // Change the surrounding keys.
                ++inferior;
                ++superior;
            }
        }
    }

    // Return a copy of this as a shared_ptr for polymorphic purpose.
    std::shared_ptr<ColorGenerator> DiscreteGradient::clone_impl() const
    {
        return std::make_shared<DiscreteGradient>(*this);
    }
}
