#ifndef COLORS_GENERATOR_H
#define COLORS_GENERATOR_H

#include <vector>
#include <SFML/Graphics.hpp>

#include "Observable.h"

namespace colors
{
    // ColorGenerator is an simple abstract class.
    // From a single float between 0 and 1, returns a color.
    // As a polymorphic class, it is generally used as a
    // 'shared_ptr<>'.
    // Note that this class is an Observable.
    class ColorGenerator : public Observable
    {
    public:
        ColorGenerator() = default;
        virtual ~ColorGenerator() {};
        
        // Interface: returns a color from a float between 0 and 1.
        virtual sf::Color get(float f) = 0;

        // Clone the current object and returns it as an object managed by a
        // 'shared_ptr'. Calls internally 'clone_impl()'. The rational behind it
        // is to have the correct object when copying or copy-constructing an
        // object havino a 'ColorGenerator' as an attribute.
        std::shared_ptr<ColorGenerator> clone() const;

    private:
        // Implements the 'clone()' method. Returns a 'shared_ptr' constructed
        // as the child class' type.
        virtual std::shared_ptr<ColorGenerator> clone_impl() const = 0;
    };

    
    //------------------------------------------------------------


    // For each float, returns a constant color.
    class ConstantColor : public ColorGenerator
    {
    public:
        // Construct a pure white ConstantColor.
        ConstantColor();
        ConstantColor(const sf::Color& color);

        // For every float 'f', returns 'color_'
        sf::Color get(float f) override;

        // Getter and setter
        const sf::Color& get_color() const;
        void set_color(const sf::Color& color);
        
    private:
        // Clone 'this' and returns it as a 'shared_ptr'.
        std::shared_ptr<ColorGenerator> clone_impl() const override;

        // The unique color returned.
        sf::Color color_;
    };


    //------------------------------------------------------------


    // Contains a set of keys defining a linear RGB gradient.
    // From the float, returns a color from this gradient.
    //
    // Invariant: the 'sanitized_keys_' are always sanitized from 'raw_keys_'.
    class LinearGradient : public ColorGenerator
    {
    public:
        // A key is a color associated to a position defined by a float. On the
        // gradient, at the key's position there will be the key's
        // color. Between two keys, at any position the color returned will be
        // the linear RGB interpolation of the adjacent keys.
        using keys = std::vector<std::pair<sf::Color, float>>;

        // Construct a two-key gradient from white to white.
        LinearGradient();

        // Create a LinearGradient with 'raw_keys_' as 'key_colors' and
        // 'sanitized_keys' as the sanitized 'key_colors'.
        // Precondition: 'key_colors' must have at least two keys.
        LinearGradient(const keys& key_colors);

        // Returns the RGB interpolation between the two adjacent keys of 'f'.
        // 'f' is automatically clamped.
        sf::Color get(float f) override;

        // Getters
        const keys& get_raw_keys() const;
        const keys& get_sanitized_keys() const;

        // Setter to raw_keys_.
        // sanitized_keys_ is update to respect the invariant.
        // Precondition: 'key_colors' must have at least two keys.
        void set_keys(const keys& keys);

    private:
        // Sanitize 'raw_keys_' into 'sanitize_keys_':
        // - The keys must be included between 0 and 1.
        // - The lowest key is always at 0 and the highest is always as 1.
        // - The keys are always sorted in the vector.
        void sanitize_keys();

        // Clone 'this' and returns it as a 'shared_ptr'.
        std::shared_ptr<ColorGenerator> clone_impl() const override;

        // The raw keys : they may be not ordered.
        keys raw_keys_;
        // The sanitized_keys_: derived from 'raw_keys_': all keys are sorted
        // from 0 to 1.
        keys sanitized_keys_;
    };


    // Contains a set of keys defining a discrete gradient.
    // Invariant:
    // - 'keys_' is always sorted with at least a first element of index '0'.
    // - 'colors_' always correspond to the 'keys_'.
    class DiscreteGradient : public ColorGenerator
    {
    public:
        // A key is a pair of a Color and an integer. The integer is the index
        // of the corresponding color. For example, a key set of
        // {{White,0},{Black,3}} means that the colors will be
        // {White, Light Gray, Dark Gray, Black}.
        using keys = std::vector<std::pair<sf::Color, size_t>>;

        // Construct a DiscreteGradient consisting of a single white color.
        DiscreteGradient();

        // Construct a DiscreteGradient respecting the invariants.
        DiscreteGradient(const keys& keys);

        // Returns a colors corresponding to a RGB interpolation between the two
        // adjacent keys of 'f'. The number of colors between the two adjacent
        // keys is fixed.
        // 'f' is automatically clamped.
        sf::Color get(float f) override;

        // Getters
        const keys& get_keys() const;
        const std::vector<sf::Color>& get_colors() const;

        // Setter
        // Set 'keys_' to 'keys' and generate the 'colors_'.
        // Precondition: 'keys' respect the invariants.
        void set_keys(keys keys);
        
    private:
        // Generate 'colors_' from 'keys_'.
        // The invariant are assumed respected.
        void generate_colors();

        // Clone 'this' and returns it as a 'shared_ptr'.
        std::shared_ptr<ColorGenerator> clone_impl() const override;
        
        // The keys. Always respect the relevant invariant.
        keys keys_;
        // The colors. Always respect the relevant invariant.
        std::vector<sf::Color> colors_;
    };
}


#endif // COLORS_GENERATOR_H
