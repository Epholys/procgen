#include <gtest/gtest.h>
#include "ColorsGenerator.h"

using namespace colors;

TEST(ColorGeneratorTest, constant_ctor)
{
    ConstantColor c {sf::Color::Blue};
    ASSERT_EQ(sf::Color::Blue, c.get_color());
}

TEST(ColorGeneratorTest, constant_setter)
{
    ConstantColor c;
    c.set_color(sf::Color::Blue);
    ASSERT_EQ(sf::Color::Blue, c.get_color());
}

TEST(ColorGeneratorTest, constant_get)
{
    ConstantColor c {sf::Color::Blue};
    ASSERT_EQ(sf::Color::Blue, c.get(0.5f));
}

TEST(ColorGeneratorTest, constant_clone)
{
    std::shared_ptr<ColorGenerator> pc = std::make_shared<ConstantColor>(sf::Color::Blue);
    auto clone = pc->clone();

    ASSERT_EQ(sf::Color::Blue, clone->get(0.5f));
}

//------------------------------------------------------------

TEST(ColorGeneratorTest, linear_ctor)
{
    LinearGradient::keys keys {{sf::Color::Red, 0},{sf::Color::Green, 0.5},{sf::Color::Blue, 1.}};
    LinearGradient l {keys};
    ASSERT_EQ(keys, l.get_raw_keys());
}

TEST(ColorGeneratorTest, linear_sanitize)
{
    LinearGradient::keys raw_keys {{sf::Color::Red, -1},{sf::Color::Green, 0.5},{sf::Color::Blue, 2}};
    LinearGradient::keys sanitized_keys {{sf::Color::Red, 0},{sf::Color::Green, 0.5},{sf::Color::Blue, 1}};
    LinearGradient l1 {raw_keys};
    ASSERT_EQ(sanitized_keys, l1.get_sanitized_keys());

    raw_keys  = {{sf::Color::Red, 0.2},{sf::Color::Green, 0.5},{sf::Color::Blue, 0.8}};
    LinearGradient l2 {raw_keys};
    ASSERT_EQ(sanitized_keys, l2.get_sanitized_keys());
}

TEST(ColorGeneratorTest, linear_setter)
{
    LinearGradient l {};
    LinearGradient::keys raw_keys {{sf::Color::Red, -1},{sf::Color::Green, 0.5},{sf::Color::Blue, 2}};
    LinearGradient::keys sanitized_keys {{sf::Color::Red, 0},{sf::Color::Green, 0.5},{sf::Color::Blue, 1}};
    l.set_keys(raw_keys);

    ASSERT_EQ(raw_keys, l.get_raw_keys());
    ASSERT_EQ(sanitized_keys, l.get_sanitized_keys());
}

TEST(ColorGeneratorTest, linear_get)
{
    LinearGradient::keys keys {{sf::Color::Red, 0},{sf::Color::Green, 0.5},{sf::Color::Blue, 1.}};
    sf::Color interpolation1 {127, 127, 0};
    sf::Color interpolation2 {0, 127, 127};
    LinearGradient l {keys};

    ASSERT_EQ(interpolation1, l.get(0.25));
    ASSERT_EQ(interpolation2, l.get(0.75));
}

TEST(ColorGeneratorTest, linear_clone)
{
    LinearGradient::keys keys {{sf::Color::Red, 0},{sf::Color::Green, 0.5},{sf::Color::Blue, 1.}};
    std::shared_ptr<ColorGenerator> pl = std::make_shared<LinearGradient>(keys);
    auto clone = pl->clone();

    ASSERT_EQ(sf::Color::Green, clone->get(0.5f));
}

//------------------------------------------------------------

TEST(ColorGeneratorTest, discrete_ctor)
{
    DiscreteGradient::keys keys {{sf::Color::Blue, 0}, {sf::Color::Red, 2}};
    DiscreteGradient d {keys};

    ASSERT_EQ(keys, d.get_keys());
}

TEST(ColorGeneratorTest, discrete_generate)
{
    DiscreteGradient::keys keys {{sf::Color::Red, 0}, {sf::Color::Green, 1}, {sf::Color::Blue, 3}};
    std::vector<sf::Color> colors {sf::Color::Red, sf::Color::Green, sf::Color(0,127,127), sf::Color::Blue};
    DiscreteGradient d {keys};

    ASSERT_EQ(colors, d.get_colors());
}

TEST(ColorGeneratorTest, discrete_setter)
{
    DiscreteGradient::keys keys {{sf::Color::Red, 0}, {sf::Color::Green, 1}, {sf::Color::Blue, 3}};
    std::vector<sf::Color> colors {sf::Color::Red, sf::Color::Green, sf::Color(0,127,127), sf::Color::Blue};
    DiscreteGradient d {};
    d.set_keys(keys);

    ASSERT_EQ(keys, d.get_keys());
    ASSERT_EQ(colors, d.get_colors());
}

TEST(ColorGeneratorTest, discrete_get)
{
    DiscreteGradient::keys keys {{sf::Color::Red, 0}, {sf::Color::Green, 1}, {sf::Color::Blue, 3}};
    std::vector<sf::Color> colors {sf::Color::Red, sf::Color::Green, sf::Color(0,127,127), sf::Color::Blue};
    DiscreteGradient d {keys};

    ASSERT_EQ(colors[0], d.get(0.2));
    ASSERT_EQ(colors[1], d.get(0.25));
    ASSERT_EQ(colors[2], d.get(0.6));
    ASSERT_EQ(colors[3], d.get(1));
}

TEST(ColorGeneratorTest, discrete_clone)
{
    DiscreteGradient::keys keys {{sf::Color::Red, 0}, {sf::Color::Green, 1}, {sf::Color::Blue, 3}};
    std::shared_ptr<ColorGenerator> pd = std::make_shared<DiscreteGradient>(keys);
    auto clone = pd->clone();

    ASSERT_EQ(sf::Color::Red, pd->get(0.2));
}
