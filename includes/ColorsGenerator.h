#ifndef COLORS_GENERATOR_H
#define COLORS_GENERATOR_H

#include <vector>
#include <SFML/Graphics.hpp>

#include "Observable.h"

namespace colors
{
    // ColorGenerator is an simple interface.
    // From a single float between 0 and 1, returns a color.
    class ColorGenerator : public Observable
    {
    public:
        ColorGenerator() = default;
        virtual ~ColorGenerator() {};
        
        // Returns a color from a float.
        virtual sf::Color get(float f) = 0;

        std::shared_ptr<ColorGenerator> clone() const;

    private:
        virtual std::shared_ptr<ColorGenerator> clone_impl() const = 0;
    };

    
    // For each float, returns a constant color.
    class ConstantColor : public ColorGenerator
    {
    public:
        ConstantColor();
        ConstantColor(const sf::Color& color);

        sf::Color get(float f) override;

        const sf::Color get_color() const;
        void set_color(const sf::Color& color);
        
    private:
        std::shared_ptr<ColorGenerator> clone_impl() const override;

        sf::Color color_;
    };


    // Contains a set of keys defining a linear gradient.
    // From the float, returns a color from this gradient.
    //
    // Invariant: the keys are always sanitized before returning the color.
    class LinearGradient : public ColorGenerator
    {
    public:
        // A key is a color associated to a position defined by a float. On the
        // gradient, at the key's position there will be the key's
        // color. Between two keys, at any position the color returned will be
        // the linear interpolation of the adjacent keys.
        using keys = std::vector<std::pair<sf::Color, float>>;

        LinearGradient();

        // Precondition: 'key_colors' must have at least two keys.
        LinearGradient(const keys& key_colors);

        sf::Color get(float f) override;

        // Getter
        const keys& get_keys() const;

        // Setter (keys are always modified externally)
        // Precondition: 'key_colors' must have at least two keys.
        void set_keys(const keys& keys);

        // Sanitizes the keys.
        // The keys must be included between 0 and 1.
        // The lowest key is always at 0 and the highest is always as 1.
        // The keys are always sorted in the vector.        
        static keys sanitize_keys(const keys& colors);

    private:
        std::shared_ptr<ColorGenerator> clone_impl() const override;

        keys key_colors_;
    };


    class DiscreteGradient : public ColorGenerator
    {
    public:
        using keys = std::vector<std::pair<sf::Color, size_t>>;

        DiscreteGradient();
        DiscreteGradient(const keys& keys);

        sf::Color get(float f) override;

        const keys& get_keys() const;
        void set_keys(keys keys);
        static std::vector<sf::Color> generate_colors(const keys& dirty_keys);
        
    private:
        static keys sanitize_keys(const keys& dirty_keys);
        std::shared_ptr<ColorGenerator> clone_impl() const override;
        
        keys keys_;
        std::vector<sf::Color> colors_;
    };
}


#endif // COLORS_GENERATOR_H
