#include <cmath>

#include "InterpretationMap.h"
#include "Turtle.h"

namespace drawing
{
    using namespace impl;
    
    void go_forward_fn(Turtle& turtle)
    {
        double dx = turtle.parameters.get_step() * std::cos(turtle.state.angle);
        double dy = turtle.parameters.get_step() * -std::sin(turtle.state.angle); // y axis point downward
        turtle.state.position += {dx, dy};
        turtle.vertices.push_back(sf::Vector2f(turtle.state.position));
    }

    void turn_right_fn(Turtle& turtle)
    {
        turtle.state.angle += turtle.parameters.get_delta_angle();
    }

    void turn_left_fn(Turtle& turtle)
    {
        turtle.state.angle -= turtle.parameters.get_delta_angle();
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
            turtle.vertices.push_back( {sf::Vector2f(turtle.stack.top().position), sf::Color::Transparent} );
            turtle.vertices.push_back( {sf::Vector2f(turtle.stack.top().position)} );
            turtle.stack.pop();
        }
    }

    InterpretationMap::InterpretationMap(const rule_map& rules)
        : RuleMap<Order>(rules)
    {
    }
    InterpretationMap::InterpretationMap(std::initializer_list<typename rule_map::value_type> init)
        : RuleMap<Order>(init)
    {        
    }
}
