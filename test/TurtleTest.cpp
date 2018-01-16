#include <vector>
#include <cmath>

#include <gtest/gtest.h>
#include <SFML/Graphics.hpp>

#include "LSystem.h"
#include "Turtle.h"
#include "math.h"

using namespace std;
using namespace lsys;
using namespace drawing;
using namespace math;

class TurtleTest :  public ::testing::Test
{
public:
    TurtleTest()
        : turtle(parameters)
        {
            // Turtle is normally initialized inside
            // drawing::compute_vertices. Manually initialized here to
            // test smaller the functions.
        }
    
    LSystem lsys { "F", { { 'F', "F+G" } } };
    interpretation_map interpretation { { 'F', go_forward },
                                        { 'G', go_forward },
                                        { '+', turn_left  } };
    // starting_position, starting_angle, delta_angle, step, n_iter
    DrawingParameters parameters { { 100, 100 }, degree_to_rad(45.f), degree_to_rad(90.f), 10, 0 };
    impl::Turtle turtle;
};

// SFML does not provide an equality operator for sf::Vertex. It is
// defined inside the 'sf' namespace to help googletest find it.
namespace sf
{
    inline bool operator== (const sf::Vertex& left, const sf::Vertex& right)
    {
        return
            left.position == right.position &&
            left.color == right.color &&
            left.texCoords == right.texCoords;
    }
}

// Test the go_forward order.
TEST_F(TurtleTest, go_forward)
{
    sf::Vertex begin { parameters.starting_position };
    float newx = parameters.step * std::cos(turtle.angle);
    float newy = parameters.step * std::sin(turtle.angle);
    sf::Vector2f end_pos = begin.position + sf::Vector2f (newx, newy);
    sf::Vertex end { end_pos } ;

    go_forward(turtle);
    
    ASSERT_EQ(turtle.vertices.at(0), begin);
    ASSERT_EQ(turtle.vertices.at(1), end);
}

// Test the turn_right order.
TEST_F(TurtleTest, turn_right)
{
    turn_right(turtle);
    
    ASSERT_FLOAT_EQ(turtle.angle, parameters.starting_angle + parameters.delta_angle);
}

// Test the turn_left order.
TEST_F(TurtleTest, turn_left)
{
    turn_left(turtle);
    
    ASSERT_FLOAT_EQ(turtle.angle, parameters.starting_angle - parameters.delta_angle);
}

// The L-system defined returns the string: "F+G" with 1 iteration.
// The turtle interpretation of "F+G" in this case is:
//   1. go_forward
//   2. turn_left
//   3. go_forward
TEST_F(TurtleTest, compute_vertices)
{
    go_forward(turtle);
    turn_left (turtle);
    go_forward(turtle);

    vector<sf::Vertex> norm { turtle.vertices.at(0),
                              turtle.vertices.at(1),
                              turtle.vertices.at(2) };

    parameters.n_iter = 1;
    auto res = compute_vertices(lsys, interpretation, parameters);

    ASSERT_EQ(res, norm);
}
