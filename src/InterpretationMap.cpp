#include "InterpretationMap.h"
#include "Turtle.h"

namespace drawing
{
    using namespace impl;
    
    void go_forward_fn(Turtle& turtle)
    {
        // OPT IDEAS :
        // - fill vec with vertices and only access x and y
        // - create intermediaire vertices with only x, y, bool (transparent)
        
        // Go forward following the direction vector.
        double dx = turtle.step * turtle.state.direction.x;
        double dy = turtle.step * -turtle.state.direction.y;
        turtle.state.position += {dx, dy};
        turtle.vertices.emplace_back(sf::Vector2f(turtle.state.position));
        turtle.iteration_of_vertices.emplace_back(turtle.iteration_vec.at(turtle.iteration_index));
    }

    void turn_left_fn(Turtle& turtle)
    {
        // Updates the direction vector.
        ext::sf::Vector2d v
        {turtle.state.direction.x * turtle.cos - turtle.state.direction.y * turtle.sin,
         turtle.state.direction.x * turtle.sin + turtle.state.direction.y * turtle.cos};
        turtle.state.direction = v;
    }

    void turn_right_fn(Turtle& turtle)
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
        if (turtle.stack.empty() || turtle.vertices.size() == 0)
        {
            // Do nothing
        }
        else
        {
            turtle.vertices.emplace_back(turtle.vertices.back().position, sf::Color::Transparent);
            turtle.state = turtle.stack.top();
            turtle.vertices.emplace_back(sf::Vector2f(turtle.state.position), sf::Color::Transparent);
            turtle.vertices.emplace_back(sf::Vector2f(turtle.state.position));
            turtle.iteration_of_vertices.emplace_back(turtle.iteration_vec.at(turtle.iteration_index));
            turtle.iteration_of_vertices.emplace_back(turtle.iteration_vec.at(turtle.iteration_index));
            turtle.iteration_of_vertices.emplace_back(turtle.iteration_vec.at(turtle.iteration_index));

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
