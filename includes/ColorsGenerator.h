#ifndef COLORS_GENERATOR_H
#define COLORS_GENERATOR_H

#include <cstdio>
#include <vector>
#include <SFML/Graphics.hpp>
#include "cereal/types/polymorphic.hpp"
#include "cereal/cereal.hpp"
#include "cereal/types/vector.hpp"
#include "cereal/archives/json.hpp"
#include "Observable.h"


namespace cereal
{
    template <class Archive,
              traits::EnableIf<traits::is_text_archive<Archive>::value> = traits::sfinae> inline
    std::string save_minimal(const Archive&, sf::Color color)
    {
        char hex_color[8];
        unsigned long r = color.r;
        unsigned long g = color.g;
        unsigned long b = color.b;
        unsigned long a = color.a;
        unsigned long  color_number = r << 24 | g << 16 | b << 8 | a;
        snprintf(hex_color, 9, "%08lX", color_number);
        std::string hex_string (hex_color);
        hex_string.insert(begin(hex_string), '#');
        return hex_string;
    }

    template <class Archive,
              traits::EnableIf<traits::is_text_archive<Archive>::value> = traits::sfinae> inline
    void load_minimal(const Archive&, sf::Color& color, const std::string& data)
    {
        std::string hex_string = data;
        bool wrong_format = false;
        for (char c : hex_string)
        {
            if (c != '#' && !std::isxdigit(c))
            {
                wrong_format = true;
                break;
            }
        }
        if (hex_string.size() != 9 || hex_string.at(0) != '#' || wrong_format)
        {
            throw RapidJSONException("Wrong hexadecimal format for sf::Color");
        }
        hex_string.erase(begin(hex_string));
        unsigned long color_number;
        sscanf(hex_string.data(), "%lX", &color_number);
        color.a = color_number & 0xff;
        color_number >>= 8;
        color.b = color_number & 0xff;
        color_number >>= 8;
        color.g = color_number & 0xff;
        color_number >>= 8;
        color.r = color_number & 0xff;
    }

    template<class Archive, class N,
             traits::EnableIf<traits::is_text_archive<Archive>::value> = traits::sfinae> inline
    void save(Archive& ar, const std::pair<sf::Color, N>& pair)
    {
        ar(cereal::make_nvp(save_minimal(ar, pair.first), pair.second));
    }

    template<class Archive, class N,
             traits::EnableIf<traits::is_text_archive<Archive>::value> = traits::sfinae> inline
    void load(Archive& ar, std::pair<sf::Color, N>& pair)
    {
        std::string color_string = ar.getNodeName();
        float key; ar(key);
        sf::Color color;
        load_minimal(ar, color, color_string);
        pair = std::make_pair(color, key);
    }

    template<class Archive, class N,
             traits::EnableIf<traits::is_text_archive<Archive>::value> = traits::sfinae> inline
    void save(Archive& ar, const std::vector<std::pair<sf::Color, N>>& color_keys)
    {
        for (auto& p : color_keys)
        {
            save(ar, p);
        }
    }
    
    template<class Archive, class N,
             traits::EnableIf<traits::is_text_archive<Archive>::value> = traits::sfinae> inline
    void load(Archive& ar, std::vector<std::pair<sf::Color, N>>& color_keys)
    {
        color_keys.clear();
        while(true)
        {
            const auto namePtr = ar.getNodeName();

            if(!namePtr)
                break;

            std::pair<sf::Color, float> key;
            load(ar, key);
            color_keys.emplace_back(key);
        }
    }
    
}

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
        explicit ConstantColor(const sf::Color& color);

        // For every float 'f', returns 'color_'
        sf::Color get(float f) override;

        // Getter and setter
        const sf::Color& get_color() const;
        void set_color(const sf::Color& color);
        
    private:
        // Clone 'this' and returns it as a 'shared_ptr'.
        std::shared_ptr<ColorGenerator> clone_impl() const override;

        friend class cereal::access;
        template<class Archive>
        void serialize(Archive& ar)
            {
                ar(cereal::make_nvp("Color", color_));
            }
        
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
        explicit LinearGradient(const keys& key_colors);

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

        friend class cereal::access;
        template<class Archive>
        void serialize(Archive& ar)
            {
                ar(cereal::make_nvp("color_keys", sanitized_keys_));
            }

        
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
        explicit DiscreteGradient(const keys& keys);

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

        friend class cereal::access;
        template<class Archive>
        void serialize(Archive& ar)
            {
                ar(cereal::make_nvp("color_keys", keys_));
            }
        
        // The keys. Always respect the relevant invariant.
        keys keys_;
        // The colors. Always respect the relevant invariant.
        std::vector<sf::Color> colors_;
    };
}

CEREAL_REGISTER_TYPE_WITH_NAME(colors::ConstantColor, "ConstantColor");
CEREAL_REGISTER_POLYMORPHIC_RELATION(colors::ColorGenerator, colors::ConstantColor)
CEREAL_REGISTER_TYPE_WITH_NAME(colors::LinearGradient, "LinearGradient");
CEREAL_REGISTER_POLYMORPHIC_RELATION(colors::ColorGenerator, colors::LinearGradient)
CEREAL_REGISTER_TYPE_WITH_NAME(colors::DiscreteGradient, "DiscreteGradient");
CEREAL_REGISTER_POLYMORPHIC_RELATION(colors::ColorGenerator, colors::DiscreteGradient)

#endif // COLORS_GENERATOR_H
