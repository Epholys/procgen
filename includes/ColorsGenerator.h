#ifndef COLORS_GENERATOR_H
#define COLORS_GENERATOR_H

#include "Indicator.h"
#include "LoadMenu.h"
#include "cereal/archives/json.hpp"
#include "cereal/cereal.hpp"
#include "cereal/types/polymorphic.hpp"
#include "cereal/types/vector.hpp"
#include "imgui/imgui.h"
#include "types.h"

#include <SFML/Graphics.hpp>
#include <cctype>
#include <cstdio>
#include <vector>

namespace cereal
{
template<class Archive, traits::EnableIf<traits::is_text_archive<Archive>::value> = traits::sfinae>
inline std::string save_minimal(const Archive&, sf::Color color)
{
    char hex_color[9];
    unsigned long r = color.r;
    unsigned long g = color.g;
    unsigned long b = color.b;
    unsigned long a = color.a;
    unsigned long color_number = r << 24 | g << 16 | b << 8 | a;
    snprintf(hex_color, 9, "%08lX", color_number);
    std::string hex_string(hex_color);
    hex_string.insert(begin(hex_string), '#');
    return hex_string;
}

template<class Archive, traits::EnableIf<traits::is_text_archive<Archive>::value> = traits::sfinae>
inline void load_minimal(const Archive&, sf::Color& color, const std::string& data)
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

template<class Archive, traits::EnableIf<traits::is_text_archive<Archive>::value> = traits::sfinae>
inline std::string save_minimal(const Archive& ar, ImVec4 imcolor)
{
    return save_minimal(ar, static_cast<sf::Color>(imcolor));
}

template<class Archive, traits::EnableIf<traits::is_text_archive<Archive>::value> = traits::sfinae>
inline void load_minimal(const Archive& ar, ImVec4& imcolor, const std::string& data)
{
    sf::Color color;
    load_minimal(ar, color, data);
    imcolor = color;
}


template<class Archive,
         class N,
         traits::EnableIf<traits::is_text_archive<Archive>::value> = traits::sfinae>
inline void save(Archive& ar, const std::pair<sf::Color, N>& pair)
{
    ar(cereal::make_nvp(save_minimal(ar, pair.first), pair.second));
}

template<class Archive,
         class N,
         traits::EnableIf<traits::is_text_archive<Archive>::value> = traits::sfinae>
inline void load(Archive& ar, std::pair<sf::Color, N>& pair)
{
    std::string color_string = ar.getNodeName();
    float key;
    ar(key);
    sf::Color color;
    load_minimal(ar, color, color_string);
    pair = std::make_pair(color, key);
}

template<class Archive,
         class N,
         traits::EnableIf<traits::is_text_archive<Archive>::value> = traits::sfinae>
inline void save(Archive& ar, const std::vector<std::pair<sf::Color, N>>& color_keys)
{
    for (auto& p : color_keys)
    {
        save(ar, p);
    }
}

template<class Archive,
         class N,
         traits::EnableIf<traits::is_text_archive<Archive>::value> = traits::sfinae>
inline void load(Archive& ar, std::vector<std::pair<sf::Color, N>>& color_keys)
{
    color_keys.clear();
    while (true)
    {
        const auto namePtr = ar.getNodeName();

        if (!namePtr)
            break;

        std::pair<sf::Color, float> key;
        load(ar, key);
        color_keys.emplace_back(key);
    }
}
} // namespace cereal

namespace colors
{
static const ImVec4 imwhite {1, 1, 1, 1};

bool is_transparent(ImVec4 imcolor);


// ColorGenerator is an simple abstract class.
// From a single float between 0 and 1, returns a color.
// As a polymorphic class, it is generally used as a
// 'shared_ptr<>'.
// Note that this class is an Observable.
class ColorGenerator : public Indicator
{
  public:
    ColorGenerator() = default;
    virtual ~ColorGenerator() = default;

    // Interface: returns a color from a float between 0 and 1.
    virtual sf::Color get(float f) = 0;

    // Clone the current object and returns it as an object managed by a
    // 'shared_ptr'. The rational behind it is to have the correct object
    // when copying or copy-constructing an object havino a 'ColorGenerator'
    // as an attribute.
    virtual std::shared_ptr<ColorGenerator> clone() const = 0;

    virtual std::string type_name() const = 0;
};


//------------------------------------------------------------


// For each float, returns a constant color.
class ConstantColor : public ColorGenerator
{
  public:
    // Construct a pure white ConstantColor.
    ConstantColor() = default;
    virtual ~ConstantColor() = default;
    ConstantColor(const ConstantColor& other) = default;
    ConstantColor(ConstantColor&& other) = default;
    ConstantColor& operator=(const ConstantColor& other) = default;
    ConstantColor& operator=(ConstantColor&& other) = default;

    explicit ConstantColor(const sf::Color& color);

    // For every float 'f', returns 'color_'
    sf::Color get(float f) override;

    // Getter and setter
    sf::Color get_color() const;       // sf::Color getter for the painters
    const ImVec4& get_imcolor() const; // ImVec4 getter for the GUI
    void set_imcolor(const ImVec4& color);

    friend class ColorGeneratorSerializer;
    virtual std::string type_name() const override;

  private:
    // Clone 'this' and returns it as a 'shared_ptr'.
    std::shared_ptr<ColorGenerator> clone() const override;

    // The unique color returned, in ImVec4 format (for ease of use with the
    // GUI).
    ImVec4 color_ {imwhite};

    friend class cereal::access;
    template<class Archive>
    void save(Archive& ar, const u32) const
    {
        ar(cereal::make_nvp("color", color_));
    }
    template<class Archive>
    void load(Archive& ar, const u32)
    {
        ar(cereal::make_nvp("color", color_));
    }
};


//------------------------------------------------------------


// Contains a set of keys defining a linear RGB gradient.
// From the float, returns a color from this gradient.
//
// Invariant: 'keys_' must:
//   - have at least two key
//   - have the first key at '0' and the last at '1'
class LinearGradient : public ColorGenerator
{
  public:
    struct Key
    {
        ImVec4 imcolor {imwhite};
        float position {0.f};
    };

    // A key is a color associated to a position defined by a float. On the
    // gradient, at the key's position there will be the key's
    // color. Between two keys, at any position the color returned will be
    // the linear RGB interpolation of the adjacent keys.
    using keys = std::vector<Key>;

    // Construct a two-key gradient from white to white.
    LinearGradient();
    virtual ~LinearGradient() = default;
    // Precondition: 'key_colors' must have at least 2 elements.
    // Note: 'key_colors' will be formated to respect the invariant.
    explicit LinearGradient(const keys& key_colors);
    LinearGradient(const LinearGradient& other) = default;
    LinearGradient(LinearGradient&& other) = default;
    LinearGradient& operator=(const LinearGradient& other) = default;
    LinearGradient& operator=(LinearGradient&& other) = default;

    // Returns the RGB interpolation between the two adjacent keys of 'f'.
    // 'f' is automatically clamped.
    sf::Color get(float f) override;

    // Getter/Setter
    const keys& get_keys() const;
    void set_keys(const keys& keys);

    friend class ColorGeneratorSerializer;
    virtual std::string type_name() const override;

  private:
    // Clone 'this' and returns it as a 'shared_ptr'.
    std::shared_ptr<ColorGenerator> clone() const override;

    // Keys
    keys keys_;


    friend class cereal::access;
    template<class Archive>
    void save(Archive& ar, u32) const
    {
        std::vector<std::pair<sf::Color, float>> keys;
        for (const auto& k : keys_)
        {
            keys.push_back({k.imcolor, k.position});
        }
        ar(cereal::make_nvp("color_keys", keys));
    }

    template<class Archive>
    void load(Archive& ar, u32)
    {
        std::vector<std::pair<sf::Color, float>> loaded_keys;
        std::vector<Key> keys;
        ar(cereal::make_nvp("color_keys", loaded_keys));

        if (loaded_keys.size() < 2)
        {
            loaded_keys = {{sf::Color::White, 0}, {sf::Color::White, 1.}};
            controller::LoadMenu::add_loading_error_message(
                "There was less than 2 keys in LinearGradient, so it is was to a default state.");
        }

        bool out_of_bound = false;
        for (const auto& k : loaded_keys)
        {
            if (k.second < 0 || k.second > 1)
            {
                out_of_bound = true;
            }
            keys.push_back({k.first, k.second});
        }

        if (out_of_bound)
        {
            controller::LoadMenu::add_loading_error_message(
                "One or more key in LinearGradient were out of bound, so they were clamped.");
        }

        set_keys(keys);

        // Avoid updating change at first creation
        poll_modification();
    }
};


// Contains a set of keys defining a discrete gradient.
// Invariant:
// - 'keys_' is always sorted with at least a two element.
// - The first element has index 0.
// - 'colors_' always correspond to the 'keys_'.
class DiscreteGradient : public ColorGenerator
{
  public:
    struct Key
    {
        ImVec4 imcolor {imwhite};
        int index {0};
    };

    // A key is a pair of a Color and an integer. The integer is the index
    // of the corresponding color. For example, a key set of
    // {{White,0},{Black,3}} means that the colors will be
    // {White, Light Gray, Dark Gray, Black}.
    using keys = std::vector<Key>;

    // Construct a DiscreteGradient consisting of a single white color.
    DiscreteGradient();
    virtual ~DiscreteGradient() = default;

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
    void set_keys(const keys& keys);

    friend class ColorGeneratorSerializer;
    virtual std::string type_name() const override;

  private:
    // Generate 'colors_' from 'keys_'.
    // The invariant are assumed respected.
    void generate_colors();

    // Clone 'this' and returns it as a 'shared_ptr'.
    std::shared_ptr<ColorGenerator> clone() const override;


    // The keys. Always respect the relevant invariant.
    keys keys_;
    // The colors. Always respect the relevant invariant.
    std::vector<sf::Color> colors_;


    friend class cereal::access;
    template<class Archive>
    void save(Archive& ar, u32) const
    {
        std::vector<std::pair<sf::Color, float>> keys;
        for (const auto& k : keys_)
        {
            keys.push_back({k.imcolor, k.index});
        }
        ar(cereal::make_nvp("color_keys", keys));
    }

    template<class Archive>
    void load(Archive& ar, u32)
    {
        std::vector<std::pair<sf::Color, int>> loaded_keys;
        std::vector<Key> keys;
        ar(cereal::make_nvp("color_keys", loaded_keys));

        if (loaded_keys.size() < 2)
        {
            loaded_keys = {{sf::Color::White, 0}, {sf::Color::White, 1}};
            controller::LoadMenu::add_loading_error_message(
                "There was less than 2 keys in DiscreteGradient, so it is was to a default state.");
        }

        bool out_of_bound = false;
        for (const auto& k : loaded_keys)
        {
            if (k.second < 0)
            {
                out_of_bound = true;
            }
            keys.push_back({k.first, k.second});
        }

        if (out_of_bound)
        {
            controller::LoadMenu::add_loading_error_message(
                "One or more key in DiscreteGradient were negative, so they were clamped.");
        }

        if (!std::is_sorted(begin(keys), end(keys), [](const auto& a, const auto& b) {
                return a.index < b.index;
            }))
        {
            controller::LoadMenu::add_loading_error_message(
                "DiscreteGradient's keys were not sorted, so they are now.");
        }

        if (keys_.front().index != 0)
        {
            controller::LoadMenu::add_loading_error_message(
                "DiscreteGradient's keys didn't have a first element, so one is designated now.");
        }

        set_keys(keys);

        // Avoid updating change at first creation
        poll_modification();
    }
};
} // namespace colors

#endif // COLORS_GENERATOR_H
