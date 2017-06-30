#include <vector>
#include <cmath>

#include <gtest/gtest.h>
#include <SFML/Graphics.hpp>

#include "LSystem.h"
#include "Turtle.h"
#include "math.h"

using namespace std;
using namespace lsys;
using namespace logo;
using namespace math;

class TurtleTest :  public ::testing::Test
{
public:
    TurtleTest()
        {
            // Normally initialized inside
            // logo::compute_vertices. Manually initialized here to
            // test smaller the functions.
            walk0.curr_pos = t0.starting_pos;
            walk0.curr_angle = t0.starting_angle;
            walk0.vertices = { { t0.starting_pos } };
        }
    
    LSystem lsys { "F", { { 'F', "F+G" } } };
    Turtle::lsys_interpretation intr { { 'F', go_forward },
                                       { 'G', go_forward },
                                       { '+', turn_left  } };
    // starting_pos, starting_angle, delta_angle, step, LSystem, lsys_interpretation
    Turtle t0 { { 100, 100 }, degree_to_rad(45), degree_to_rad(90), 10, lsys, intr };
    Walk walk0;
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
    sf::Vertex begin { t0.starting_pos };
    float newx = t0.step * std::cos(walk0.curr_angle);
    float newy = t0.step * std::sin(walk0.curr_angle);
    sf::Vector2f end_pos = begin.position + sf::Vector2f (newx, newy );
    sf::Vertex end { end_pos } ;

    go_forward(t0, walk0);
    
    ASSERT_EQ(walk0.vertices.at(0), begin);
    ASSERT_EQ(walk0.vertices.at(1), end);
}

// Test the turn_right order.
TEST_F(TurtleTest, turn_right)
{
    turn_right(t0, walk0);
    
    ASSERT_FLOAT_EQ(walk0.curr_angle, t0.starting_angle + t0.delta_angle);
}

// Test the turn_left order.
TEST_F(TurtleTest, turn_left)
{
    turn_left(t0, walk0);
    
    ASSERT_FLOAT_EQ(walk0.curr_angle, t0.starting_angle - t0.delta_angle);
}

// The L-system defined returns the string: "F+G" with 1 iteration.
// The turtle interpretation of "F+G" in this case is:
//   1. go_forward
//   2. turn_left
//   3. go_forward
TEST_F(TurtleTest, compute_vertices)
{
    go_forward(t0, walk0);
    turn_left (t0, walk0);
    go_forward(t0, walk0);

    vector<sf::Vertex> norm { walk0.vertices.at(0),
                              walk0.vertices.at(1),
                              walk0.vertices.at(2) };
    
    auto res = compute_vertices(t0, 1);

    ASSERT_EQ(res, norm);
}
