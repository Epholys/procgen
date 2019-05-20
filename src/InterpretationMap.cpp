#include <cmath>

#include "InterpretationMap.h"
#include "Turtle.h"

namespace drawing
{
    using namespace impl;
    
    void go_forward_fn(Turtle& turtle)
    {
        // Go forward following the direction vector.
        turtle.vertices.push_back(sf::Vector2f(turtle.state.position));
        turtle.iteration_of_vertices.push_back(turtle.iteration_vec.at(turtle.iteration_index));
        double dx = turtle.parameters.get_step() * turtle.state.direction.x;
        double dy = turtle.parameters.get_step() * -turtle.state.direction.y;
        turtle.state.position += {dx, dy};
        turtle.vertices.push_back(sf::Vector2f(turtle.state.position));
        turtle.iteration_of_vertices.push_back(turtle.iteration_vec.at(turtle.iteration_index));
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
        if (turtle.stack.empty() || turtle.vertices.size() == 0)
        {
            // Do nothing
        }
        else
        {
            turtle.vertices.push_back( {turtle.vertices.back().position, sf::Color::Transparent} );
            turtle.state = turtle.stack.top();
            turtle.vertices.push_back( {sf::Vector2f(turtle.state.position), sf::Color::Transparent} );
            turtle.iteration_of_vertices.push_back(turtle.iteration_vec.at(turtle.iteration_index));
            turtle.iteration_of_vertices.push_back(turtle.iteration_vec.at(turtle.iteration_index));

            turtle.stack.pop();
        }
    }

    void jitter_angle_fn(impl::Turtle& turtle)
    {
        // TODO: do not randomize the current angle, just the base angle
        // TOOD: global rule, the first branching randomize also the angle for
        // the next branches, while the last branch randomize only its
        // branch. adequate behavior ?
        turtle.cos = std::cos(turtle.parameters.get_delta_angle() *
                              math::random_real(math::default_random_generator_, .6, 1.4));
        turtle.sin = std::sin(turtle.parameters.get_delta_angle() *
                              math::random_real(math::default_random_generator_, .6, 1.4));
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
