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
            // drawing::compute_paths. Manually initialized here to
            // test smaller the functions.
        }
    
    LSystem lsys { "F", { { 'F', "F+G" } } };
    InterpretationMap interpretation { { 'F', go_forward },
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
    float newx = parameters.step * std::cos(turtle.state.angle);
    float newy = parameters.step * std::sin(turtle.state.angle);
    sf::Vector2f end_pos = begin.position + sf::Vector2f (newx, newy);
    sf::Vertex end { end_pos } ;

    go_forward_fn(turtle);
    
    ASSERT_EQ(turtle.paths.back().at(0), begin);
    ASSERT_EQ(turtle.paths.back().at(1), end);
}

// Test the turn_right order.
TEST_F(TurtleTest, turn_right)
{
    turn_right_fn(turtle);
    
    ASSERT_FLOAT_EQ(turtle.state.angle, parameters.starting_angle + parameters.delta_angle);
}

// Test the turn_left order.
TEST_F(TurtleTest, turn_left)
{
    turn_left_fn(turtle);
    
    ASSERT_FLOAT_EQ(turtle.state.angle, parameters.starting_angle - parameters.delta_angle);
}

// Test the save_position and load_position order.
TEST_F(TurtleTest, stack_test)
{
    save_position_fn(turtle);
    const auto& saved_state = turtle.stack.top();
    ASSERT_EQ(saved_state.position, turtle.state.position);
    ASSERT_EQ(saved_state.angle, turtle.state.angle);

    go_forward_fn(turtle);
    
    auto paths_size = turtle.paths.size();
    load_position_fn(turtle);
    ASSERT_EQ(paths_size+1, turtle.paths.size());
    ASSERT_EQ(saved_state.position, turtle.state.position);
    ASSERT_EQ(saved_state.angle, turtle.state.angle);

}

// The L-system defined returns the string: "F+G" with 1 iteration.
// The turtle interpretation of "F+G" in this case is:
//   1. go_forward
//   2. turn_left
//   3. go_forward
TEST_F(TurtleTest, compute_paths)
{
    go_forward_fn(turtle);
    turn_left_fn (turtle);
    go_forward_fn(turtle);

    std::vector<std::vector<sf::Vertex>> norm { { turtle.paths.back().at(0),
                                                  turtle.paths.back().at(1),
                                                  turtle.paths.back().at(2) } };
    

    parameters.n_iter = 1;
    auto res = compute_path(lsys, interpretation, parameters);

    ASSERT_EQ(res, norm);
}
