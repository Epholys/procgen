#include <vector>
#include <gtest/gtest.h>
#include <SFML/Graphics.hpp>

#include "VertexPainterWrapper.h"
#include "VertexPainterComposite.h"
#include "VertexPainterConstant.h"
#include "VertexPainterIteration.h"
#include "VertexPainterLinear.h"
#include "VertexPainterRadial.h"
#include "VertexPainterRandom.h"
#include "VertexPainterSequential.h"
#include "VertexPainterSerializer.h"

using namespace colors;

std::vector<sf::Vertex> generate_grid (int size)
{
    std::vector<sf::Vertex> grid;
    for(int i=0; i<size*size; ++i)
    {
        int x = i % size;
        int y = i / size;
        grid.push_back({{float(x), float(y)}, sf::Color::White});
    }
    return grid;
}

std::vector<std::uint8_t> generate_iterations (int size)
{
    std::vector<std::uint8_t> iterations;
    for(int i=0; i<size*size; ++i)
    {
        iterations.push_back(i/size);
    }
    return iterations;
}

std::vector<bool> generate_transparency (int size)
{
    std::vector<bool> transparent;
    for(int i=0; i<size*size; ++i)
    {
        transparent.push_back(false);
    }
    return transparent;
}


struct default_vertices
{
    static constexpr int grid_size = 4;
    std::vector<sf::Vertex> grid = generate_grid(grid_size);
    std::vector<std::uint8_t> iterations = generate_iterations(grid_size);
    std::vector<bool> transparent = generate_transparency(grid_size);
    int max_iter = grid_size-1;
    sf::FloatRect bounding_box {0, 0, grid_size-1, grid_size-1};
} vertices;

TEST(VertexPainter, Constant)
{
    ColorGeneratorWrapper colors (std::make_shared<ConstantColor>(sf::Color::Red));
    VertexPainterConstant painter (colors);
    std::vector<sf::Vertex> grid = vertices.grid;
    painter.paint_vertices(grid,
                           vertices.iterations,
                           vertices.transparent,
                           vertices.max_iter,
                           vertices.bounding_box);

    for (auto v : grid)
    {
        ASSERT_EQ(sf::Color::Red, v.color);
    }
}
TEST(VertexPainter, ConstantSerialization)
{
   ColorGeneratorWrapper colors (std::make_shared<ConstantColor>(sf::Color::Red));
    VertexPainterConstant opainter (colors);
    VertexPainterConstant ipainter (colors);

    std::stringstream ss;
    {
        cereal::JSONOutputArchive ar (ss);
        ar(opainter);
    }
    {
        cereal::JSONInputArchive ar (ss);
        ar(ipainter);
    }

    std::vector<sf::Vertex> grid = vertices.grid;
    ipainter.paint_vertices(grid,
                            vertices.iterations,
                            vertices.transparent,
                            vertices.max_iter,
                            vertices.bounding_box);

    for (auto v : grid)
    {
        ASSERT_EQ(sf::Color::Red, v.color);
    }
}

TEST(VertexPainter, Sequential)
{
    std::array<sf::Color, vertices.grid_size> colors {sf::Color::Red, sf::Color::Blue, sf::Color::Yellow, sf::Color::Green};
    ColorGeneratorWrapper colors_gen (
            std::make_shared<DiscreteGradient>(
                DiscreteGradient::keys({{colors[0], 0}, {colors[1], 1}, {colors[2], 2},{colors[3], 3}})));
    VertexPainterSequential painter (colors_gen);
    painter.set_factor(2);
    std::vector<sf::Vertex> grid = vertices.grid;
    painter.paint_vertices(grid,
                           vertices.iterations,
                           vertices.transparent,
                           vertices.max_iter,
                           vertices.bounding_box);

    int size = vertices.grid_size;
    int size_2 = size * size;
    for (int i=0; i<size_2; ++i)
    {

        ASSERT_EQ(colors[((i*2)/size)%size], grid[i].color);
    }
}
TEST(VertexPainter, SequentialSerialization)
{
    std::array<sf::Color, vertices.grid_size> colors {sf::Color::Red, sf::Color::Blue, sf::Color::Yellow, sf::Color::Green};
    ColorGeneratorWrapper colors_gen (
            std::make_shared<DiscreteGradient>(
                DiscreteGradient::keys({{colors[0], 0}, {colors[1], 1}, {colors[2], 2},{colors[3], 3}})));
    VertexPainterSequential opainter (colors_gen);
    opainter.set_factor(2);
    VertexPainterSequential ipainter;

    std::stringstream ss;
    {
        cereal::JSONOutputArchive ar (ss);
        ar(opainter);
    }
    {
        cereal::JSONInputArchive ar (ss);
        ar(ipainter);
    }

    std::vector<sf::Vertex> grid = vertices.grid;
    ipainter.paint_vertices(grid,
                            vertices.iterations,
                            vertices.transparent,
                            vertices.max_iter,
                            vertices.bounding_box);

    int size = vertices.grid_size;
    int size_2 = size * size;
    for (int i=0; i<size_2; ++i)
    {

        ASSERT_EQ(colors[((i*2)/size)%size], grid[i].color);
    }
}

TEST(VertexPainter, Iteration)
{
    std::array<sf::Color, vertices.grid_size> colors {sf::Color::Red, sf::Color::Blue, sf::Color::Yellow, sf::Color::Green};
    ColorGeneratorWrapper colors_gen (
            std::make_shared<DiscreteGradient>(
                DiscreteGradient::keys({{colors[0], 0}, {colors[1], 1}, {colors[2], 2},{colors[3], 3}})));
    VertexPainterIteration painter (colors_gen);
    std::vector<sf::Vertex> grid = vertices.grid;
    painter.paint_vertices(grid,
                           vertices.iterations,
                           vertices.transparent,
                           vertices.max_iter,
                           vertices.bounding_box);

    int size = vertices.grid_size;
    int size_2 = size * size;
    for (int i=0; i<size_2; ++i)
    {
         ASSERT_EQ(colors[i/size], grid[i].color);
    }
}
TEST(VertexPainter, IterationSerialization)
{
    std::array<sf::Color, vertices.grid_size> colors {sf::Color::Red, sf::Color::Blue, sf::Color::Yellow, sf::Color::Green};
    ColorGeneratorWrapper colors_gen (
            std::make_shared<DiscreteGradient>(
                DiscreteGradient::keys({{colors[0], 0}, {colors[1], 1}, {colors[2], 2},{colors[3], 3}})));
    VertexPainterIteration opainter (colors_gen);
    VertexPainterIteration ipainter;

    std::stringstream ss;
    {
        cereal::JSONOutputArchive ar (ss);
        ar(opainter);
    }
    {
        cereal::JSONInputArchive ar (ss);
        ar(ipainter);
    }

    std::vector<sf::Vertex> grid = vertices.grid;
    ipainter.paint_vertices(grid,
                            vertices.iterations,
                            vertices.transparent,
                            vertices.max_iter,
                            vertices.bounding_box);

    int size = vertices.grid_size;
    int size_2 = size * size;
    for (int i=0; i<size_2; ++i)
    {
         ASSERT_EQ(colors[i/size], grid[i].color);
    }
}

TEST(VertexPainter, Linear)
{
    std::array<sf::Color, vertices.grid_size> colors {sf::Color::Red, sf::Color::Blue, sf::Color::Green, sf::Color::Yellow};
    ColorGeneratorWrapper colors_gen (
            std::make_shared<DiscreteGradient>(
                DiscreteGradient::keys({{colors[0], 0}, {colors[1], 1}, {colors[2], 2}, {colors[3], 3}})));
    VertexPainterLinear painter (colors_gen);
    painter.set_angle(90);
    std::vector<sf::Vertex> grid = vertices.grid;
    painter.paint_vertices(grid,
                           vertices.iterations,
                           vertices.transparent,
                           vertices.max_iter,
                           vertices.bounding_box);

    std::vector<sf::Color> expected_colors =
        { sf::Color::Yellow, sf::Color::Yellow, sf::Color::Yellow, sf::Color::Yellow,
          sf::Color::Green , sf::Color::Green , sf::Color::Green , sf::Color::Green ,
          sf::Color::Blue  , sf::Color::Blue  , sf::Color::Blue  , sf::Color::Blue  ,
          sf::Color::Red   , sf::Color::Red   , sf::Color::Red   , sf::Color::Red   };
    int size = vertices.grid_size;
    int size_2 = size * size;
    for (int i=0; i<size_2; ++i)
    {
        ASSERT_EQ(expected_colors[i], grid[i].color);
    }
}
TEST(VertexPainter, LinearSerialization)
{
    std::array<sf::Color, vertices.grid_size> colors {sf::Color::Red, sf::Color::Blue, sf::Color::Green, sf::Color::Yellow};
    ColorGeneratorWrapper colors_gen (
            std::make_shared<DiscreteGradient>(
                DiscreteGradient::keys({{colors[0], 0}, {colors[1], 1}, {colors[2], 2}, {colors[3], 3}})));
    VertexPainterLinear opainter (colors_gen);
    opainter.set_angle(90);
    VertexPainterLinear ipainter;

    std::stringstream ss;
    {
        cereal::JSONOutputArchive ar (ss);
        ar(opainter);
    }
    {
        cereal::JSONInputArchive ar (ss);
        ar(ipainter);
    }

    std::vector<sf::Vertex> grid = vertices.grid;
    ipainter.paint_vertices(grid,
                            vertices.iterations,
                            vertices.transparent,
                            vertices.max_iter,
                            vertices.bounding_box);

    std::vector<sf::Color> expected_colors =
        { sf::Color::Yellow, sf::Color::Yellow, sf::Color::Yellow, sf::Color::Yellow,
          sf::Color::Green , sf::Color::Green , sf::Color::Green , sf::Color::Green ,
          sf::Color::Blue  , sf::Color::Blue  , sf::Color::Blue  , sf::Color::Blue  ,
          sf::Color::Red   , sf::Color::Red   , sf::Color::Red   , sf::Color::Red   };
    int size = vertices.grid_size;
    int size_2 = size * size;
    for (int i=0; i<size_2; ++i)
    {
        ASSERT_EQ(expected_colors[i], grid[i].color);
    }
}

TEST(VertexPainter, Radial)
{
    std::array<sf::Color, vertices.grid_size> colors {sf::Color::Red, sf::Color::Blue, sf::Color::Green};
    ColorGeneratorWrapper colors_gen (
            std::make_shared<DiscreteGradient>(
                DiscreteGradient::keys({{colors[0], 0}, {colors[1], 1}, {colors[2], 2}})));
    VertexPainterRadial painter (colors_gen);
    painter.set_center({1/3., 2/3.});
    std::vector<sf::Vertex> grid = vertices.grid;
    painter.paint_vertices(grid,
                           vertices.iterations,
                           vertices.transparent,
                           vertices.max_iter,
                           vertices.bounding_box);

    std::vector<sf::Color> expected_colors =
        { sf::Color::Blue , sf::Color::Blue , sf::Color::Blue , sf::Color::Green,
          sf::Color::Blue , sf::Color::Red  , sf::Color::Blue , sf::Color::Green,
          sf::Color::Blue , sf::Color::Blue , sf::Color::Blue , sf::Color::Green,
          sf::Color::Green, sf::Color::Green, sf::Color::Green, sf::Color::Green };
    int size = vertices.grid_size;
    int size_2 = size * size;
    for (int i=0; i<size_2; ++i)
    {
        ASSERT_EQ(expected_colors[i], grid[i].color);
    }
}
TEST(VertexPainter, RadialSerialization)
{
    std::array<sf::Color, vertices.grid_size> colors {sf::Color::Red, sf::Color::Blue, sf::Color::Green};
    ColorGeneratorWrapper colors_gen(
            std::make_shared<DiscreteGradient>(
                DiscreteGradient::keys({{colors[0], 0}, {colors[1], 1}, {colors[2], 2}})));
    VertexPainterRadial opainter (colors_gen);
    opainter.set_center({1/3., 2/3.});
    VertexPainterRadial ipainter;


    std::stringstream ss;
    {
        cereal::JSONOutputArchive ar (ss);
        ar(opainter);
    }
    {
        cereal::JSONInputArchive ar (ss);
        ar(ipainter);
    }


    std::vector<sf::Vertex> grid = vertices.grid;
    ipainter.paint_vertices(grid,
                            vertices.iterations,
                            vertices.transparent,
                            vertices.max_iter,
                            vertices.bounding_box);

    std::vector<sf::Color> expected_colors =
        { sf::Color::Blue , sf::Color::Blue , sf::Color::Blue , sf::Color::Green,
          sf::Color::Blue , sf::Color::Red  , sf::Color::Blue , sf::Color::Green,
          sf::Color::Blue , sf::Color::Blue , sf::Color::Blue , sf::Color::Green,
          sf::Color::Green, sf::Color::Green, sf::Color::Green, sf::Color::Green };
    int size = vertices.grid_size;
    int size_2 = size * size;
    for (int i=0; i<size_2; ++i)
    {
        ASSERT_EQ(expected_colors[i], grid[i].color);
    }
}

TEST(VertexPainter, Random)
{
    std::array<sf::Color, vertices.grid_size> colors {sf::Color::Red, sf::Color::Blue, sf::Color::Yellow, sf::Color::Green};
    ColorGeneratorWrapper colors_gen (
            std::make_shared<DiscreteGradient>(
                DiscreteGradient::keys({{colors[0], 0}, {colors[1], 1}, {colors[2], 2},{colors[3], 3}})));
    VertexPainterRandom painter (colors_gen);
    painter.set_block_size(vertices.grid_size);
    std::vector<sf::Vertex> grid = vertices.grid;
    painter.paint_vertices(grid,
                           vertices.iterations,
                           vertices.transparent,
                           vertices.max_iter,
                           vertices.bounding_box);

    int size = vertices.grid_size;
    for (int i=0; i<size; ++i)
    {
        std::array<sf::Color, vertices.grid_size> grid_colors;
        for (int j=0; j<size; ++j)
        {
            grid_colors[j] = grid[i*size + j].color;
        }
        auto first = grid_colors[0];
        ASSERT_TRUE(std::all_of(begin(grid_colors), end(grid_colors),
                                [first](auto col){return col == first;}));
        ASSERT_TRUE(std::any_of(begin(colors), end(colors),
                                [first](auto col){return col == first;}));
    }
}
TEST(VertexPainter, RandomSerialization)
{
    std::array<sf::Color, vertices.grid_size> colors {sf::Color::Red, sf::Color::Blue, sf::Color::Yellow, sf::Color::Green};
    ColorGeneratorWrapper colors_gen (
            std::make_shared<DiscreteGradient>(
                DiscreteGradient::keys({{colors[0], 0}, {colors[1], 1}, {colors[2], 2},{colors[3], 3}})));
    VertexPainterRandom opainter (colors_gen);
    opainter.set_block_size(vertices.grid_size);
    VertexPainterRandom ipainter;


    std::stringstream ss;
    {
        cereal::JSONOutputArchive ar (ss);
        ar(opainter);
    }
    {
        cereal::JSONInputArchive ar (ss);
        ar(ipainter);
    }


    std::vector<sf::Vertex> grid = vertices.grid;
    ipainter.paint_vertices(grid,
                            vertices.iterations,
                            vertices.transparent,
                            vertices.max_iter,
                            vertices.bounding_box);

    int size = vertices.grid_size;
    for (int i=0; i<size; ++i)
    {
        std::array<sf::Color, vertices.grid_size> grid_colors;
        for (int j=0; j<size; ++j)
        {
            grid_colors[j] = grid[i*size + j].color;
        }
        auto first = grid_colors[0];
        ASSERT_TRUE(std::all_of(begin(grid_colors), end(grid_colors),
                                [first](auto col){return col == first;}));
        ASSERT_TRUE(std::any_of(begin(colors), end(colors),
                                [first](auto col){return col == first;}));
    }
}

TEST(VertexPainter, Composite)
{
    sf::Color color {sf::Color::Red};
    std::array<sf::Color, 2> colors1 {sf::Color::Green, sf::Color::Blue};
    std::array<sf::Color, 2> colors2 {sf::Color::Yellow, sf::Color::Magenta};
    std::array<sf::Color, 2> colors3 {sf::Color::Cyan, sf::Color::White};
    ColorGeneratorWrapper constant_gen (
            std::make_shared<ConstantColor>(color));
    ColorGeneratorWrapper discrete_gen1(
            std::make_shared<DiscreteGradient>(
                DiscreteGradient::keys({{colors1[0], 0}, {colors1[1], 1}})));
    ColorGeneratorWrapper discrete_gen2 (
            std::make_shared<DiscreteGradient>(
                DiscreteGradient::keys({{colors2[0], 0}, {colors2[1], 1}})));
    ColorGeneratorWrapper discrete_gen3 (
            std::make_shared<DiscreteGradient>(
                DiscreteGradient::keys({{colors3[0], 0}, {colors3[1], 1}})));

    VertexPainterWrapper constant_wrapper (std::make_shared<VertexPainterConstant>(constant_gen));
    VertexPainterWrapper radial_wrapper (std::make_shared<VertexPainterRadial>(discrete_gen1));
    VertexPainterWrapper linear_wrapper (std::make_shared<VertexPainterLinear>(discrete_gen2));
    VertexPainterWrapper sequential_wrapper (std::make_shared<VertexPainterSequential>(discrete_gen3));

    std::vector<sf::Vertex> grid = vertices.grid;
    VertexPainterComposite composite;
    composite.set_main_painter(VertexPainterWrapper(std::make_shared<VertexPainterIteration>()));
    composite.set_child_painters({constant_wrapper, radial_wrapper, linear_wrapper, sequential_wrapper});
    composite.paint_vertices(grid,
                             vertices.iterations,
                             vertices.transparent,
                             vertices.max_iter,
                             vertices.bounding_box);

    std::vector<sf::Color> expected_colors =
        { sf::Color::Red   , sf::Color::Red    , sf::Color::Red    , sf::Color::Red,
          sf::Color::Blue  , sf::Color::Green  , sf::Color::Green  , sf::Color::Blue,
          sf::Color::Yellow, sf::Color::Yellow , sf::Color::Magenta, sf::Color::Magenta,
          sf::Color::Cyan  , sf::Color::Cyan   , sf::Color::White  , sf::Color::White };

    int size = vertices.grid_size;
    int size_2 = size * size;
    for (int i=0; i<size_2; ++i)
    {
        ASSERT_EQ(expected_colors[i], grid[i].color);
    }
}
TEST(VertexPainter, CompositeSerialization)
{
    sf::Color color {sf::Color::Red};
    std::array<sf::Color, 2> colors1 {sf::Color::Green, sf::Color::Blue};
    std::array<sf::Color, 2> colors2 {sf::Color::Yellow, sf::Color::Magenta};
    std::array<sf::Color, 2> colors3 {sf::Color::Cyan, sf::Color::White};
    ColorGeneratorWrapper constant_gen (
            std::make_shared<ConstantColor>(color));
    ColorGeneratorWrapper discrete_gen1(
            std::make_shared<DiscreteGradient>(
                DiscreteGradient::keys({{colors1[0], 0}, {colors1[1], 1}})));
    ColorGeneratorWrapper discrete_gen2 (
            std::make_shared<DiscreteGradient>(
                DiscreteGradient::keys({{colors2[0], 0}, {colors2[1], 1}})));
    ColorGeneratorWrapper discrete_gen3 (
            std::make_shared<DiscreteGradient>(
                DiscreteGradient::keys({{colors3[0], 0}, {colors3[1], 1}})));

    VertexPainterWrapper constant_wrapper (std::make_shared<VertexPainterConstant>(constant_gen));
    VertexPainterWrapper radial_wrapper (std::make_shared<VertexPainterRadial>(discrete_gen1));
    VertexPainterWrapper linear_wrapper (std::make_shared<VertexPainterLinear>(discrete_gen2));
    VertexPainterWrapper sequential_wrapper (std::make_shared<VertexPainterSequential>(discrete_gen3));

    VertexPainterComposite ocomposite;
    ocomposite.set_main_painter(VertexPainterWrapper(std::make_shared<VertexPainterIteration>()));
    ocomposite.set_child_painters({constant_wrapper, radial_wrapper, linear_wrapper, sequential_wrapper});
    VertexPainterComposite icomposite;


    std::stringstream ss;
    {
        cereal::JSONOutputArchive ar (ss);
        ar(ocomposite);
    }
    {
        cereal::JSONInputArchive ar (ss);
        ar(icomposite);
    }


    std::vector<sf::Vertex> grid = vertices.grid;
    icomposite.paint_vertices(grid,
                              vertices.iterations,
                              vertices.transparent,
                              vertices.max_iter,
                              vertices.bounding_box);

    std::vector<sf::Color> expected_colors =
        { sf::Color::Red   , sf::Color::Red    , sf::Color::Red    , sf::Color::Red,
          sf::Color::Blue  , sf::Color::Green  , sf::Color::Green  , sf::Color::Blue,
          sf::Color::Yellow, sf::Color::Yellow , sf::Color::Magenta, sf::Color::Magenta,
          sf::Color::Cyan , sf::Color::Cyan  , sf::Color::White  , sf::Color::White };

    int size = vertices.grid_size;
    int size_2 = size * size;
    for (int i=0; i<size_2; ++i)
    {
        ASSERT_EQ(expected_colors[i], grid[i].color);
    }
}

TEST(VertexPainter, PolymorphicSerialization)
{
    std::array<sf::Color, vertices.grid_size> colors1 {sf::Color::Red, sf::Color::Blue, sf::Color::Yellow, sf::Color::Green};
    ColorGeneratorWrapper colors_gen1 (
            std::make_shared<DiscreteGradient>(
                DiscreteGradient::keys({{colors1[0], 0}, {colors1[1], 1}, {colors1[2], 2},{colors1[3], 3}})));
    auto orandom = std::make_shared<VertexPainterRandom>(colors_gen1);
    orandom->set_block_size(vertices.grid_size);

    ColorGeneratorWrapper colors2 (
            std::make_shared<ConstantColor>(sf::Color::Red));
    auto oconstant = std::make_shared<VertexPainterConstant>(colors2);

    std::array<sf::Color, vertices.grid_size> colors3 {sf::Color::Red, sf::Color::Blue, sf::Color::Green};
    ColorGeneratorWrapper colors_gen3 (
            std::make_shared<DiscreteGradient>(
                DiscreteGradient::keys({{colors3[0], 0}, {colors3[1], 1}, {colors3[2], 2}})));
    auto olinear = std::make_shared<VertexPainterLinear> (colors_gen3);
    olinear->set_angle(45);

    std::shared_ptr<VertexPainter> irandom;
    std::shared_ptr<VertexPainter> iconstant;
    std::shared_ptr<VertexPainter> ilinear;

    std::stringstream ss_random;
    std::stringstream ss_constant;
    std::stringstream ss_linear;
    {
        cereal::JSONOutputArchive oarchive_random (ss_random);
        cereal::JSONOutputArchive oarchive_constant  (ss_constant);
        cereal::JSONOutputArchive oarchive_linear (ss_linear);

        VertexPainterSerializer serializer_random (orandom);
        VertexPainterSerializer serializer_constant (oconstant);
        VertexPainterSerializer serializer_linear (olinear);

        oarchive_random(serializer_random);
        oarchive_constant(serializer_constant);
        oarchive_linear(serializer_linear);
    }


    VertexPainterSerializer serializer_random;
    VertexPainterSerializer serializer_constant;
    VertexPainterSerializer serializer_linear;
    {
        cereal::JSONInputArchive iarchive_random (ss_random);
        cereal::JSONInputArchive iarchive_constant (ss_constant);
        cereal::JSONInputArchive iarchive_linear (ss_linear);

        iarchive_random(serializer_random);
        iarchive_constant(serializer_constant);
        iarchive_linear(serializer_linear);
    }
    irandom = serializer_random.get_serialized();
    iconstant = serializer_constant.get_serialized();
    ilinear = serializer_linear.get_serialized();

    ASSERT_TRUE(std::dynamic_pointer_cast<VertexPainterRandom>(irandom));
    ASSERT_TRUE(std::dynamic_pointer_cast<VertexPainterConstant>(iconstant));
    ASSERT_TRUE(std::dynamic_pointer_cast<VertexPainterLinear>(ilinear));

}
