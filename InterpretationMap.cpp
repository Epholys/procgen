#include <cmath>

#include "InterpretationMap.h"
#include "Turtle.h"

namespace drawing
{
    using namespace impl;
    
    void go_forward_fn(Turtle& turtle)
    {
        float dx = turtle.parameters.step * std::cos(turtle.state.angle);
        float dy = turtle.parameters.step * std::sin(turtle.state.angle);
        turtle.state.position += {dx, dy};
        turtle.vertices.push_back(turtle.state.position);
    }

    void turn_right_fn(Turtle& turtle)
    {
        turtle.state.angle += turtle.parameters.delta_angle;
    }

    void turn_left_fn(Turtle& turtle)
    {
        turtle.state.angle -= turtle.parameters.delta_angle;
    }

    void save_position_fn(Turtle& turtle)
    {
        turtle.stack.push(turtle.state);
    }

    void load_position_fn(Turtle& turtle)
    {
        if (turtle.stack.empty())
        {
            // Do nothing
        }
        else
        {
            turtle.vertices.push_back( {turtle.vertices.back().position, sf::Color::Transparent} );
            turtle.state.position = turtle.stack.top().position;
            turtle.state.angle = turtle.stack.top().angle;
            turtle.vertices.push_back( {turtle.stack.top().position, sf::Color::Transparent} );
            turtle.vertices.push_back( {turtle.stack.top().position} );
            turtle.stack.pop();
        }
    }
}
