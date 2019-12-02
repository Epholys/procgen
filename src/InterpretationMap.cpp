#include "InterpretationMap.h"
#include "Turtle.h"

namespace drawing
{
    using namespace impl;

    void go_forward_fn(Turtle& turtle)
    {
        // Go forward following the direction vector.
        double dx = turtle.step * turtle.state.direction.x;
        double dy = turtle.step * -turtle.state.direction.y;
        turtle.state.position += {dx, dy};
        turtle.vertices.emplace_back(sf::Vector2f(turtle.state.position));
        turtle.iteration_of_vertices.push_back(turtle.iteration);
        turtle.transparent.push_back(false);
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

            turtle.iteration_of_vertices.push_back(turtle.iteration);
            turtle.iteration_of_vertices.push_back(turtle.iteration);
            turtle.iteration_of_vertices.push_back(turtle.iteration);

            turtle.transparent.push_back(true);
            turtle.transparent.push_back(true);
            turtle.transparent.push_back(false);

            turtle.stack.pop();
        }
    }

    InterpretationMap::InterpretationMap(const Rules& rules)
        : RuleMap<Order>(rules)
    {
    }
    InterpretationMap::InterpretationMap(std::initializer_list<typename Rules::value_type> init)
        : RuleMap<Order>(init)
    {
    }
}
