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
        turtle.vertices_recursion.push_back(turtle.str_recursion.at(turtle.recursion_index));
        ++turtle.recursion_index;
    }

    void turn_right_fn(Turtle& turtle)
    {
        // Updates the direction vector.
        ext::sf::Vector2d v
        {turtle.state.direction.x * turtle.cos - turtle.state.direction.y * turtle.sin,
         turtle.state.direction.x * turtle.sin + turtle.state.direction.y * turtle.cos};
        turtle.state.direction = v;
        ++turtle.recursion_index;
    }

    void turn_left_fn(Turtle& turtle)
    {
        // Updates the direction vector.
        ext::sf::Vector2d v
        {turtle.state.direction.x * turtle.cos - turtle.state.direction.y * (-turtle.sin),
         turtle.state.direction.x * (-turtle.sin) + turtle.state.direction.y * turtle.cos};
        turtle.state.direction = v;
        ++turtle.recursion_index;
    }

    void save_position_fn(Turtle& turtle)
    {
        turtle.stack.push(turtle.state);
        ++turtle.recursion_index;
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
            turtle.vertices.push_back( {sf::Vector2f(turtle.state.position), sf::Color::Transparent} );
            turtle.vertices.push_back( {sf::Vector2f(turtle.state.position)} );

            turtle.vertices_recursion.push_back(turtle.str_recursion.at(turtle.recursion_index));
            turtle.vertices_recursion.push_back(turtle.str_recursion.at(turtle.recursion_index));
            turtle.vertices_recursion.push_back(turtle.str_recursion.at(turtle.recursion_index));

            turtle.stack.pop();
        }
        ++turtle.recursion_index;
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
