#include <cmath>
#include <sstream>

#include <gtest/gtest.h>
#include <SFML/Graphics.hpp>
#include "cereal/archives/json.hpp"

#include "LSystem.h"
#include "Turtle.h"
#include "InterpretationMap.h"


using namespace std;
using namespace drawing;
using namespace math;

class DrawingTest :  public ::testing::Test
{
public:
    DrawingTest()
        {
            // Turtle is normally initialized inside
            // drawing::compute_vertices. Manually initialized here to
            // test smaller the functions.
        }
    
    LSystem lsys { "F", { { 'F', "F+G" } }, "F" };
    InterpretationMap interpretation { { 'F', go_forward },
                                        { 'G', go_forward },
                                        { '+', turn_left  },
                                        { '-', turn_right },
                                        { '[', save_position },
                                        { ']', load_position } };
    // starting_position, starting_angle, delta_angle, step, n_iter
    DrawingParameters parameters { { 100, 100 }, 0, degree_to_rad(90.), 10, 0 };
    impl::Turtle turtle {parameters, {1, 1, 1}};
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
TEST_F(DrawingTest, go_forward)
{
    sf::Vertex begin ( {0.f, 0.f} );
    float newx = parameters.get_step() * std::cos(parameters.get_starting_angle());
    float newy = parameters.get_step() * std::sin(parameters.get_starting_angle());
    sf::Vector2f end_pos = begin.position + sf::Vector2f (newx, newy);
    sf::Vertex end { end_pos };
    std::vector<int> expected_rec {1,1};

    go_forward_fn(turtle);
    
    ASSERT_EQ(turtle.vertices.at(0), begin);
    ASSERT_EQ(turtle.vertices.at(1), end);
    ASSERT_EQ(turtle.recursion_index, 1);
    ASSERT_EQ(turtle.vertices_recursion, expected_rec);
}

// Test the turn_right order.
TEST_F(DrawingTest, turn_right)
{
    turn_right_fn(turtle);

    ext::sf::Vector2d direction { 0, 1 };
    ASSERT_NEAR(turtle.state.direction.x, direction.x, 1e-10);
    ASSERT_NEAR(turtle.state.direction.y, direction.y, 1e-10);

    std::vector<int> expected_rec {1};
    ASSERT_EQ(turtle.recursion_index, 1);
    ASSERT_EQ(turtle.vertices_recursion, expected_rec);
}

// Test the turn_left order.
TEST_F(DrawingTest, turn_left)
{
    turn_left_fn(turtle);
    
    ext::sf::Vector2d direction { 0, -1 };
    ASSERT_NEAR(turtle.state.direction.x, direction.x, 1e-10);
    ASSERT_NEAR(turtle.state.direction.y, direction.y, 1e-10);

    std::vector<int> expected_rec {1};
    ASSERT_EQ(turtle.recursion_index, 1);
    ASSERT_EQ(turtle.vertices_recursion, expected_rec);
}

// Test the save_position and load_position order.
TEST_F(DrawingTest, stack_test)
{
    save_position_fn(turtle);
    const auto& saved_state = turtle.stack.top();
    ASSERT_EQ(saved_state.position, turtle.state.position);
    ASSERT_EQ(saved_state.direction, turtle.state.direction);

    go_forward_fn(turtle);
    load_position_fn(turtle);

    ASSERT_EQ(saved_state.position, turtle.state.position);
    ASSERT_EQ(saved_state.direction, turtle.state.direction);

    std::vector<int> expected_rec {1,1,1,1,1};
    ASSERT_EQ(turtle.recursion_index, 3);
    ASSERT_EQ(turtle.vertices_recursion, expected_rec);
}

// The L-system defined returns the string: "F+G" with 1 iteration.
// The turtle interpretation of "F+G" in this case is:
//   1. go_forward
//   2. turn_left
//   3. go_forward
TEST_F(DrawingTest, compute_paths)
{
    go_forward_fn(turtle);
    turn_left_fn (turtle);
    go_forward_fn(turtle);

    std::vector<sf::Vertex> norm { turtle.vertices.at(0),
                                   turtle.vertices.at(1),
                                   turtle.vertices.at(2)  };
    

    parameters.set_n_iter(1);
    auto [str, rec] = compute_vertices(lsys, interpretation, parameters);

    ASSERT_EQ(str, norm);

    
    std::vector<int> expected_rec {1,1,1};
    ASSERT_EQ(turtle.recursion_index, 3);
    ASSERT_EQ(rec, expected_rec);

}

TEST_F(DrawingTest, serialization)
{
    InterpretationMap imap;
        
    std::stringstream ss;
    {
        cereal::JSONOutputArchive oarchive (ss);
        oarchive(interpretation);
    }
    {
        cereal::JSONInputArchive iarchive (ss);
        iarchive(imap);
    }

    ASSERT_EQ(interpretation.get_rules(), imap.get_rules());
}
