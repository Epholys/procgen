#include <gtest/gtest.h>
#include "ColorsGenerator.h"
#include "ColorsGeneratorWrapper.h"
#include "ColorsGeneratorSerializer.h"

using namespace colors;

TEST(ColorGeneratorTest, color_serialization)
{
    std::stringstream ss;
    
    sf::Color color (0x11, 0x22, 0x33, 0x44);
    std::string expected_str = "#11223344";
    // Unused generic archive
    cereal::JSONOutputArchive ar (ss);
    
    auto tested_str = cereal::save_minimal(ar, color);
    ASSERT_EQ(expected_str, tested_str);

    sf::Color tested_color;
    cereal::load_minimal(ar, tested_color, tested_str);
    ASSERT_EQ(color, tested_color);
}

TEST(ColorGeneratorTest, pair_color_serialization)
{
    sf::Color color (0x11, 0x22, 0x33, 0x44);
    std::pair<sf::Color, int> expected_pair {color, 42};
    std::pair<sf::Color, int> tested_pair {sf::Color::Transparent, -1};

    std::stringstream ss;
    {
        cereal::JSONOutputArchive ar (ss);
        ar(expected_pair);
    }
    {
        cereal::JSONInputArchive ar (ss);
        ar(tested_pair);
    }

    ASSERT_EQ(expected_pair, tested_pair);
}

TEST(ColorGeneratorTest, vector_pair_serialization)
{
    std::vector<std::pair<sf::Color, int>> expected_vector =
        { {sf::Color::Red, 7      },
          {sf::Color::Blue, 42    },
          {sf::Color::Green, 1337 } };
    std::vector<std::pair<sf::Color, int>> tested_vector;

    std::stringstream ss;
    {
        cereal::JSONOutputArchive ar (ss);
        ar(expected_vector);
    }
    {
        cereal::JSONInputArchive ar (ss);
        ar(tested_vector);
    }

    ASSERT_EQ(expected_vector, tested_vector);
}

//---------------------------------------------------------------------------------------

// Lots of not-so-useful tests, as the constructors are
// defaulted-generated. They do no harm, so we keep it.
TEST(ColorGeneratorTest, constant_ctor)
{
    ConstantColor c {sf::Color::Blue};
    ASSERT_EQ(sf::Color::Blue, c.get_color());
}
TEST(ColorGeneratorTest, constant_copy_ctor)
{
    ConstantColor c {sf::Color::Blue};
    ConstantColor c_copy (c);
    ASSERT_EQ(c.get_color(), c_copy.get_color());
}
TEST(ColorGeneratorTest, constant_assigned)
{
    ConstantColor c {sf::Color::Blue};
    ConstantColor c_assigned {sf::Color::Red};
    c_assigned = c;
    ASSERT_EQ(c.get_color(), c_assigned.get_color());
}
TEST(ColorGeneratorTest, constant_move_ctor)
{
    ConstantColor c {sf::Color::Blue};
    ConstantColor c_moved {std::move(c)};
    ASSERT_EQ(sf::Color::Blue, c_moved.get_color());
}
TEST(ColorGeneratorTest, constant_moved)
{
    ConstantColor c {sf::Color::Blue};
    ConstantColor c_moved {sf::Color::Red};
    c_moved = std::move(c);
    ASSERT_EQ(sf::Color::Blue, c_moved.get_color());
}

TEST(ColorGeneratorTest, constant_setter)
{
    ConstantColor c;
    c.set_imcolor(sf::Color::Blue);
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

TEST(ColorGeneratorTest, constant_serialization)
{
    ConstantColor oconstant {sf::Color::Red};
    ConstantColor iconstant;

    std::stringstream ss;
    {
        cereal::JSONOutputArchive oarchive (ss);
        oarchive(oconstant);
    }
    {
        cereal::JSONInputArchive iarchive (ss);
        iarchive(iconstant);
    }

    ASSERT_EQ(oconstant.get_color(), iconstant.get_color());
}

//------------------------------------------------------------

namespace colors
{
    inline bool operator== (const LinearGradient::Key left, const LinearGradient::Key right)
    {
        return sf::Color(left.imcolor) == sf::Color(right.imcolor) && left.position == right.position;
    }
}

// Lots of useless tests, as the constructors are defaulted-generated.
TEST(ColorGeneratorTest, linear_ctor)
{
    LinearGradient::keys keys {{sf::Color::Red, 0},{sf::Color::Green, 0.5},{sf::Color::Blue, 1.}};
    LinearGradient l {keys};
    ASSERT_EQ(keys, l.get_keys());
}
TEST(ColorGeneratorTest, linear_raw_ctor)
{
    LinearGradient::keys keys {{sf::Color::Red, 0},{sf::Color::Green, 0.5},{sf::Color::Blue, 1.}};
    LinearGradient::keys raw_keys {{sf::Color::Red, -1},{sf::Color::Green, 0.5},{sf::Color::Blue, 2}};
    LinearGradient l {raw_keys};
    ASSERT_EQ(keys, l.get_keys());
}
TEST(ColorGeneratorTest, linear_copy_ctor)
{
    LinearGradient::keys keys {{sf::Color::Red, 0},{sf::Color::Green, 0.5},{sf::Color::Blue, 1.}};
    LinearGradient l {keys};
    LinearGradient l_copy (l);
    ASSERT_EQ(l.get_keys(), l_copy.get_keys());
}
TEST(ColorGeneratorTest, linear_assigned)
{
    LinearGradient::keys keys {{sf::Color::Red, 0},{sf::Color::Green, 0.5},{sf::Color::Blue, 1.}};
    LinearGradient l {keys};
    LinearGradient l_assigned {{{sf::Color::Yellow, 0}, {sf::Color::Magenta, 1}}};
    l_assigned = l;
    ASSERT_EQ(l.get_keys(), l_assigned.get_keys());
}
TEST(ColorGeneratorTest, linear_move_ctor)
{
    LinearGradient::keys keys {{sf::Color::Red, 0},{sf::Color::Green, 0.5},{sf::Color::Blue, 1.}};
    LinearGradient l {keys};
    LinearGradient l_moved {std::move(l)};
    ASSERT_EQ(keys, l_moved.get_keys());
}
TEST(ColorGeneratorTest, linear_moved)
{
    LinearGradient::keys keys {{sf::Color::Red, 0},{sf::Color::Green, 0.5},{sf::Color::Blue, 1.}};
    LinearGradient l {keys};
    LinearGradient l_moved {{{sf::Color::Yellow, 0}, {sf::Color::Magenta, 1}}};
    l_moved = std::move(l);
    ASSERT_EQ(keys, l_moved.get_keys());
}

TEST(ColorGeneratorTest, linear_setter)
{
    LinearGradient::keys sanitized_keys {{sf::Color::Red, 0},{sf::Color::Green, 0.5},{sf::Color::Blue, 1}};

    LinearGradient::keys raw_keys {{sf::Color::Red, -1},{sf::Color::Green, 0.5},{sf::Color::Blue, 2}};
    LinearGradient l {raw_keys};
    ASSERT_EQ(sanitized_keys, l.get_keys());

    raw_keys  = {{sf::Color::Red, 0.2},{sf::Color::Green, 0.5},{sf::Color::Blue, 0.8}};
    l.set_keys(raw_keys);
    ASSERT_EQ(sanitized_keys, l.get_keys());
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

TEST(ColorGeneratorTest, linear_serialization)
{
    LinearGradient::keys keys {{sf::Color::Red, 0},{sf::Color::Green, 0.5},{sf::Color::Blue, 1.}};
    LinearGradient olinear {keys};
    LinearGradient ilinear;

    std::stringstream ss;
    {
        cereal::JSONOutputArchive oarchive (ss);
        oarchive(olinear);
    }
    {
        cereal::JSONInputArchive iarchive (ss);
        iarchive(ilinear);
    }

    ASSERT_EQ(olinear.get_keys(), ilinear.get_keys());
}

//------------------------------------------------------------

namespace colors
{
    inline bool operator== (const DiscreteGradient::Key left, const DiscreteGradient::Key right)
    {
        return sf::Color(left.imcolor) == sf::Color(right.imcolor) && left.index == right.index;
    }
}

// Lots of useless tests, as the constructors are defaulted-generated.
TEST(ColorGeneratorTest, discrete_ctor)
{
    DiscreteGradient::keys keys {{sf::Color::Blue, 0}, {sf::Color::Red, 2}};
    DiscreteGradient d {keys};

    ASSERT_EQ(keys, d.get_keys());
}
TEST(ColorGeneratorTest, discrete_copy_ctor)
{
    DiscreteGradient::keys keys {{sf::Color::Blue, 0}, {sf::Color::Red, 2}};
    DiscreteGradient d {keys};
    DiscreteGradient d_copy (d);
    ASSERT_EQ(d.get_keys(), d_copy.get_keys());
}
TEST(ColorGeneratorTest, discrete_assigned)
{
    DiscreteGradient::keys keys {{sf::Color::Blue, 0}, {sf::Color::Red, 2}};
    DiscreteGradient d {keys};
    DiscreteGradient d_assigned {{{sf::Color::Yellow, 0},{sf::Color::Magenta, 1}}};
    d_assigned = d;
    ASSERT_EQ(d.get_keys(), d_assigned.get_keys());
}
TEST(ColorGeneratorTest, discrete_move_ctor)
{
    DiscreteGradient::keys keys {{sf::Color::Blue, 0}, {sf::Color::Red, 2}};
    DiscreteGradient d {keys};
    DiscreteGradient d_moved {std::move(d)};
    ASSERT_EQ(keys, d_moved.get_keys());
}
TEST(ColorGeneratorTest, discrete_moved)
{ 
    DiscreteGradient::keys keys {{sf::Color::Blue, 0}, {sf::Color::Red, 2}};
    DiscreteGradient d {keys};
    DiscreteGradient d_moved {{{sf::Color::Yellow, 0},{sf::Color::Magenta, 1}}};
    d_moved = std::move(d);
    ASSERT_EQ(keys, d_moved.get_keys());
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

TEST(ColorGeneratorTest, discrete_serialization)
{
    DiscreteGradient::keys keys {{sf::Color::Red, 0}, {sf::Color::Green, 1}, {sf::Color::Blue, 3}};
    DiscreteGradient odiscrete {keys};
    DiscreteGradient idiscrete;

    std::stringstream ss;
    {
        cereal::JSONOutputArchive oarchive (ss);
        oarchive(odiscrete);
    }
    {
        cereal::JSONInputArchive iarchive (ss);
        iarchive(idiscrete);
    }

    ASSERT_EQ(odiscrete.get_keys(), idiscrete.get_keys());
    ASSERT_EQ(odiscrete.get_colors(), idiscrete.get_colors());
}

//------------------------------------------------------------

TEST(ColorGeneratorTest, polymorphic_serialization)
{
    LinearGradient::keys linear_keys {{sf::Color::Red, 0},{sf::Color::Green, 0.5},{sf::Color::Blue, 1.}};
    DiscreteGradient::keys discrete_keys {{sf::Color::Red, 0}, {sf::Color::Green, 1}, {sf::Color::Blue, 3}};

    std::shared_ptr<ColorGenerator> oconstant = std::make_shared<ConstantColor>(sf::Color::Red);
    std::shared_ptr<ColorGenerator> olinear = std::make_shared<LinearGradient>(linear_keys);
    std::shared_ptr<ColorGenerator> odiscrete = std::make_shared<DiscreteGradient>(discrete_keys);

    std::shared_ptr<ColorGenerator> iconstant;
    std::shared_ptr<ColorGenerator> ilinear;
    std::shared_ptr<ColorGenerator> idiscrete;

    std::stringstream ss_constant;
    std::stringstream ss_linear;
    std::stringstream ss_discrete;
    {
        cereal::JSONOutputArchive oarchive_constant (ss_constant);
        cereal::JSONOutputArchive oarchive_linear (ss_linear);
        cereal::JSONOutputArchive oarchive_gradient (ss_discrete);
        
        ColorGeneratorSerializer serializer_constant (oconstant);
        ColorGeneratorSerializer serializer_linear (olinear);
        ColorGeneratorSerializer serializer_discrete (odiscrete);

        oarchive_constant(serializer_constant);
        oarchive_linear(serializer_linear);
        oarchive_gradient(serializer_discrete);
    }


    ColorGeneratorSerializer serializer_constant;
    ColorGeneratorSerializer serializer_linear;
    ColorGeneratorSerializer serializer_discrete;
    {
        cereal::JSONInputArchive iarchive_constant (ss_constant);
        cereal::JSONInputArchive iarchive_linear (ss_linear);
        cereal::JSONInputArchive iarchive_gradient (ss_discrete);

        iarchive_constant(serializer_constant);
        iarchive_linear(serializer_linear);
        iarchive_gradient(serializer_discrete);
    }
    iconstant = serializer_constant.get_serialized();
    ilinear = serializer_linear.get_serialized();
    idiscrete = serializer_discrete.get_serialized();

    ASSERT_TRUE(std::dynamic_pointer_cast<ConstantColor>(iconstant));
    ASSERT_TRUE(std::dynamic_pointer_cast<LinearGradient>(ilinear));
    ASSERT_TRUE(std::dynamic_pointer_cast<DiscreteGradient>(idiscrete));
}

//------------------------------------------------------------

class Obs : public Observer<ColorGeneratorWrapper>
{
public:
    using O = Observer<ColorGeneratorWrapper>;

    explicit Obs(std::shared_ptr<ColorGeneratorWrapper> wrapper)
        : O{wrapper}
        {add_callback( [this](){ ++notify_count; });}

    operator bool() const
        {
            return notify_count == 1;
        }

private:
    int notify_count {0};
};
    
TEST(ColorGeneratorTest, wrapper_ctor)
{
    auto generator = std::make_shared<ConstantColor>(sf::Color::Red);
    ColorGeneratorWrapper wrapper (generator);
    ASSERT_EQ(generator.get(), wrapper.unwrap().get());
}
TEST(ColorGeneratorTest, wrapper_copy_ctor)
{
    auto generator = std::make_shared<ConstantColor>(sf::Color::Red);
    ColorGeneratorWrapper wrapper (generator);
    ColorGeneratorWrapper wrapper_copy (wrapper);
    ASSERT_NE(wrapper.unwrap().get(), wrapper_copy.unwrap().get());

    auto constant = std::dynamic_pointer_cast<ConstantColor>(wrapper_copy.unwrap());
    ASSERT_TRUE(constant);
    ASSERT_EQ(generator->get_color(), constant->get_color());
}
TEST(ColorGeneratorTest, wrapper_assigned)
{
    auto generator = std::make_shared<ConstantColor>(sf::Color::Red);
    ColorGeneratorWrapper wrapper (generator);
    ColorGeneratorWrapper wrapper_assigned;
    wrapper_assigned = wrapper;
    ASSERT_NE(wrapper.unwrap().get(), wrapper_assigned.unwrap().get());

    auto constant = std::dynamic_pointer_cast<ConstantColor>(wrapper_assigned.unwrap());
    ASSERT_TRUE(constant);
    ASSERT_EQ(generator->get_color(), constant->get_color());
}
TEST(ColorGeneratorTest, wrapper_moved_ctor)
{
    auto generator = std::make_shared<ConstantColor>(sf::Color::Red);
    ColorGeneratorWrapper wrapper (generator);
    ColorGeneratorWrapper wrapper_moved (std::move(wrapper));
    ASSERT_EQ(generator.get(), wrapper_moved.unwrap().get());
}
TEST(ColorGeneratorTest, wrapper_moved)
{
    auto generator = std::make_shared<ConstantColor>(sf::Color::Red);
    ColorGeneratorWrapper wrapper (generator);
    ColorGeneratorWrapper wrapper_moved;
    wrapper_moved = std::move(wrapper);
    ASSERT_EQ(generator.get(), wrapper_moved.unwrap().get());
}

TEST(ColorGeneratorTest, wrapper_wrap)
{
    auto generator = std::make_shared<ConstantColor>(sf::Color::Red);
    auto wrapper = std::make_shared<ColorGeneratorWrapper>();
    Obs observer (wrapper);
    wrapper->wrap(generator);
    ASSERT_EQ(generator.get(), wrapper->unwrap().get());
    ASSERT_TRUE(observer);
}
