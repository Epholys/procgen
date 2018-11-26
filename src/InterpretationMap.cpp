#include <cmath>

#include "InterpretationMap.h"
#include "Turtle.h"

namespace drawing
{
    using namespace impl;
    
    void go_forward_fn(Turtle& turtle)
    {
        // Go forward following the direction vector.
        double dx = turtle.parameters.get_step() * turtle.state.direction.x;
        double dy = turtle.parameters.get_step() * -turtle.state.direction.y;
        turtle.state.position += {dx, dy};
        turtle.vertices.push_back(sf::Vector2f(turtle.state.position));
    }

    void turn_right_fn(Turtle& turtle)
    {
        // Updates the direction vector.
        ext::sf::Vector2d v
        {turtle.state.direction.x * turtle.cos - turtle.state.direction.y * turtle.sin,
         turtle.state.direction.x * turtle.sin + turtle.state.direction.y * turtle.cos};
        turtle.state.direction = v;
    }

    void turn_left_fn(Turtle& turtle)
    {
        // Updates the direction vector.
        ext::sf::Vector2d v
        {turtle.state.direction.x * turtle.cos - turtle.state.direction.y * (-turtle.sin),
         turtle.state.direction.x * (-turtle.sin) + turtle.state.direction.y * turtle.cos};
        turtle.state.direction = v;
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
            turtle.state = turtle.stack.top();
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
