#include <vector>
#include <gtest/gtest.h>
#include <SFML/Graphics.hpp>

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
        int x = i / size;
        int y = i % size;
        grid.push_back({{x, y}, sf::Color::White});
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
