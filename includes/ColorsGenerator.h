#ifndef COLORS_GENERATOR_H
#define COLORS_GENERATOR_H

#include <vector>
#include <SFML/Graphics.hpp>

namespace colors
{
    class ColorGenerator
    {
    public:
        ColorGenerator() = default;
        virtual ~ColorGenerator() {};
        
        virtual sf::Color get(float f) = 0;
    };

    class ConstantColor : public ColorGenerator
    {
    public:
        ConstantColor(const sf::Color& color);

        sf::Color get(float f) override;

    private:
        sf::Color color_;
    };

    class LinearGradient : public ColorGenerator
    {
    public:
        using keys = std::vector<std::pair<sf::Color, float>>;
        
        LinearGradient(const keys& key_colors);

        sf::Color get(float f) override;
        
    private:
        keys key_colors_;
    };
}


#endif // COLORS_GENERATOR_H
