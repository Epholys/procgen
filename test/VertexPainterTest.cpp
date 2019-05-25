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

std::vector<int> generate_iterations (int size)
{
    std::vector<int> iterations;
    for(int i=0; i<size*size; ++i)
    {
        iterations.push_back(i/size);
    }
    return iterations;
}


struct default_vertices
{
    static constexpr int grid_size = 4;
    std::vector<sf::Vertex> grid = generate_grid(grid_size);
    std::vector<int> iterations = generate_iterations(grid_size);
    int max_iter = grid_size-1;
    sf::FloatRect bounding_box {0, 0, grid_size-1, grid_size-1};
} vertices;

TEST(VertexPainter, Constant)
{
    auto colors =
        std::make_shared<ColorGeneratorWrapper>(
            std::make_shared<ConstantColor>(sf::Color::Red));
    VertexPainterConstant painter (colors);
    std::vector<sf::Vertex> grid = vertices.grid;
    painter.paint_vertices(grid, vertices.iterations, vertices.max_iter, vertices.bounding_box);

    for (auto v : grid)
    {
        ASSERT_EQ(sf::Color::Red, v.color);
    }
}

TEST(VertexPainter, Sequential)
{
    std::array<sf::Color, vertices.grid_size> colors {sf::Color::Red, sf::Color::Blue, sf::Color::Yellow, sf::Color::Green};
    auto colors_gen =
        std::make_shared<ColorGeneratorWrapper>(
            std::make_shared<DiscreteGradient>(
                DiscreteGradient::keys({{colors[0], 0}, {colors[1], 1}, {colors[2], 2},{colors[3], 3}})));
    VertexPainterSequential painter (colors_gen);
    painter.set_factor(2);
    std::vector<sf::Vertex> grid = vertices.grid;
    painter.paint_vertices(grid, vertices.iterations, vertices.max_iter, vertices.bounding_box);

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
    auto colors_gen =
        std::make_shared<ColorGeneratorWrapper>(
            std::make_shared<DiscreteGradient>(
                DiscreteGradient::keys({{colors[0], 0}, {colors[1], 1}, {colors[2], 2},{colors[3], 3}})));
    VertexPainterIteration painter (colors_gen);
    std::vector<sf::Vertex> grid = vertices.grid;
    painter.paint_vertices(grid, vertices.iterations, vertices.max_iter, vertices.bounding_box);
 
    int size = vertices.grid_size;
    int size_2 = size * size;
    for (int i=0; i<size_2; ++i)
    {
         ASSERT_EQ(colors[i/size], grid[i].color);
    }
}

TEST(VertexPainter, Linear)
{
    std::array<sf::Color, vertices.grid_size> colors {sf::Color::Red, sf::Color::Blue, sf::Color::Green};
    auto colors_gen =
        std::make_shared<ColorGeneratorWrapper>(
            std::make_shared<DiscreteGradient>(
                DiscreteGradient::keys({{colors[0], 0}, {colors[1], 1}, {colors[2], 2}})));
    VertexPainterLinear painter (colors_gen);
    painter.set_center({1/3., 2/3.});
    painter.set_angle(45);
    std::vector<sf::Vertex> grid = vertices.grid;
    painter.paint_vertices(grid, vertices.iterations, vertices.max_iter, vertices.bounding_box);
 
    std::vector<sf::Color> expected_colors =
        { sf::Color::Blue, sf::Color::Red , sf::Color::Red  , sf::Color::Red,
          sf::Color::Red , sf::Color::Red , sf::Color::Red  , sf::Color::Blue,
          sf::Color::Red , sf::Color::Red , sf::Color::Blue , sf::Color::Green,
          sf::Color::Red , sf::Color::Blue, sf::Color::Green, sf::Color::Green };
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
    auto colors_gen =
        std::make_shared<ColorGeneratorWrapper>(
            std::make_shared<DiscreteGradient>(
                DiscreteGradient::keys({{colors[0], 0}, {colors[1], 1}, {colors[2], 2}})));
    VertexPainterRadial painter (colors_gen);
    painter.set_center({1/3., 2/3.});
    std::vector<sf::Vertex> grid = vertices.grid;
    painter.paint_vertices(grid, vertices.iterations, vertices.max_iter, vertices.bounding_box);
 
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
    auto colors_gen =
        std::make_shared<ColorGeneratorWrapper>(
            std::make_shared<DiscreteGradient>(
                DiscreteGradient::keys({{colors[0], 0}, {colors[1], 1}, {colors[2], 2},{colors[3], 3}})));
    VertexPainterRandom painter (colors_gen);
    painter.set_block_size(vertices.grid_size);
    std::vector<sf::Vertex> grid = vertices.grid;
    painter.paint_vertices(grid, vertices.iterations, vertices.max_iter, vertices.bounding_box);
 
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
    auto constant_gen =
        std::make_shared<ColorGeneratorWrapper>(
            std::make_shared<ConstantColor>(color));
    auto discrete_gen1 =
        std::make_shared<ColorGeneratorWrapper>(
            std::make_shared<DiscreteGradient>(
                DiscreteGradient::keys({{colors1[0], 0}, {colors1[1], 1}})));
    auto discrete_gen2 =
        std::make_shared<ColorGeneratorWrapper>(
            std::make_shared<DiscreteGradient>(
                DiscreteGradient::keys({{colors2[0], 0}, {colors2[1], 1}})));
    auto discrete_gen3 =
        std::make_shared<ColorGeneratorWrapper>(
            std::make_shared<DiscreteGradient>(
                DiscreteGradient::keys({{colors3[0], 0}, {colors3[1], 1}})));

    auto constant_wrapper = std::make_shared<VertexPainterWrapper>(std::make_shared<VertexPainterConstant>(constant_gen));
    auto radial_wrapper = std::make_shared<VertexPainterWrapper>(std::make_shared<VertexPainterRadial>(discrete_gen1));
    auto linear_painter = std::make_shared<VertexPainterLinear>(discrete_gen2);
    linear_painter->set_angle(90);
    auto linear_wrapper = std::make_shared<VertexPainterWrapper>(linear_painter);
    auto sequential_wrapper = std::make_shared<VertexPainterWrapper>(std::make_shared<VertexPainterSequential>(discrete_gen3));

    std::vector<sf::Vertex> grid = vertices.grid;
    VertexPainterComposite composite;
    composite.set_main_painter(std::make_shared<VertexPainterWrapper>(std::make_shared<VertexPainterIteration>()));
    composite.set_child_painters({constant_wrapper, radial_wrapper, linear_wrapper, sequential_wrapper});
    composite.paint_vertices(grid, vertices.iterations, vertices.max_iter, vertices.bounding_box);
    
    std::vector<sf::Color> expected_colors =
        { sf::Color::Red   , sf::Color::Red    , sf::Color::Red    , sf::Color::Red,
          sf::Color::Blue  , sf::Color::Green  , sf::Color::Green  , sf::Color::Blue,
          sf::Color::Magenta, sf::Color::Yellow, sf::Color::Yellow, sf::Color::Magenta,
          sf::Color::Cyan , sf::Color::Cyan  , sf::Color::White  , sf::Color::White };

    int size = vertices.grid_size;
    int size_2 = size * size;
    for (int i=0; i<size_2; ++i)
    {
        ASSERT_EQ(expected_colors[i], grid[i].color);
    }
}

