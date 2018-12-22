#include <cmath>

#include "InterpretationMap.h"
#include "Turtle.h"

namespace drawing
{
    using namespace impl;
    
    void go_forward_fn(Turtle& turtle)
    {
        // TODO TriangleStrip

        ext::sf::Vector2d normal = {-turtle.state.direction.y, turtle.state.direction.x};
        ext::sf::Vector2d downleft = turtle.state.position_left;
        ext::sf::Vector2d downright = turtle.state.position_right;

        // Go forward following the direction vector.
        // turtle.vertices.push_back(sf::Vector2f(turtle.state.position));
        // turtle.iteration_of_vertices.push_back(turtle.iteration_vec.at(turtle.iteration_index));
        //double new_width = turtle.state.old_width * turtle.parameters.get_width_ratio();
        double difference = (turtle.state.old_width - turtle.state.width) / 2.;
        double dx1 = turtle.parameters.get_step() * turtle.state.direction.x - normal.x * difference;
        double dy1 = turtle.parameters.get_step() * -turtle.state.direction.y + normal.y * difference;
        double dx2 = turtle.parameters.get_step() * turtle.state.direction.x + normal.x * difference;
        double dy2 = turtle.parameters.get_step() * -turtle.state.direction.y - normal.y * difference;
        ext::sf::Vector2d upleft = downleft + ext::sf::Vector2d{dx1, dy1};
        ext::sf::Vector2d upright = downright + ext::sf::Vector2d{dx2, dy2};

        turtle.vertices.push_back(sf::Vector2f(downleft));
        turtle.vertices.push_back(sf::Vector2f(upleft));
        turtle.vertices.push_back(sf::Vector2f(downright));
        turtle.vertices.push_back(sf::Vector2f(downright));
        turtle.vertices.push_back(sf::Vector2f(upleft));
        turtle.vertices.push_back(sf::Vector2f(upright));
        turtle.iteration_of_vertices.push_back(turtle.iteration_vec.at(turtle.iteration_index));
        turtle.iteration_of_vertices.push_back(turtle.iteration_vec.at(turtle.iteration_index));
        turtle.iteration_of_vertices.push_back(turtle.iteration_vec.at(turtle.iteration_index));
        turtle.iteration_of_vertices.push_back(turtle.iteration_vec.at(turtle.iteration_index));
        turtle.iteration_of_vertices.push_back(turtle.iteration_vec.at(turtle.iteration_index));
        turtle.iteration_of_vertices.push_back(turtle.iteration_vec.at(turtle.iteration_index));

        // std::cout << "old_Width: " << turtle.state.old_width << " ; width : " << turtle.state.width << "\n";
        
        turtle.state.position_left = upleft;
        turtle.state.position_right = upright;
        turtle.state.old_width = turtle.state.width;
    }

    void turn_right_fn(Turtle& turtle)
    {
        // Updates the direction vector.
        ext::sf::Vector2d d
        {turtle.state.direction.x * turtle.cos - turtle.state.direction.y * turtle.sin,
         turtle.state.direction.x * turtle.sin + turtle.state.direction.y * turtle.cos};
        turtle.state.direction = d;

        ext::sf::Vector2d v {turtle.state.position_right - turtle.state.position_left};
        v = {v.x * turtle.cos - v.y * -turtle.sin,
             v.x * -turtle.sin + v.y * turtle.cos};
        v += turtle.state.position_left;

        turtle.vertices.push_back(sf::Vector2f(turtle.state.position_left));
        turtle.vertices.push_back(sf::Vector2f(turtle.state.position_right));
        turtle.vertices.push_back(sf::Vector2f(v));
        turtle.iteration_of_vertices.push_back(turtle.iteration_vec.at(turtle.iteration_index));
        turtle.iteration_of_vertices.push_back(turtle.iteration_vec.at(turtle.iteration_index));
        turtle.iteration_of_vertices.push_back(turtle.iteration_vec.at(turtle.iteration_index));

        turtle.state.position_right = v;

    }

    void turn_left_fn(Turtle& turtle)
    {
        // Updates the direction vector.
        ext::sf::Vector2d d
        {turtle.state.direction.x * turtle.cos - turtle.state.direction.y * (-turtle.sin),
         turtle.state.direction.x * (-turtle.sin) + turtle.state.direction.y * turtle.cos};
        turtle.state.direction = d;

        ext::sf::Vector2d v {turtle.state.position_left - turtle.state.position_right};
        v = {v.x * turtle.cos - v.y * turtle.sin,
             v.x * turtle.sin + v.y * turtle.cos};
        v += turtle.state.position_right;

        turtle.vertices.push_back(sf::Vector2f(turtle.state.position_left));
        turtle.vertices.push_back(sf::Vector2f(turtle.state.position_right));
        turtle.vertices.push_back(sf::Vector2f(v));
        turtle.iteration_of_vertices.push_back(turtle.iteration_vec.at(turtle.iteration_index));
        turtle.iteration_of_vertices.push_back(turtle.iteration_vec.at(turtle.iteration_index));
        turtle.iteration_of_vertices.push_back(turtle.iteration_vec.at(turtle.iteration_index));


        turtle.state.position_left = v;
    }

    // TODO change "position"
    void save_position_fn(Turtle& turtle)
    {
        turtle.stack.push(turtle.state);
        turtle.state.width = turtle.state.width * turtle.parameters.get_width_ratio();
    }

    void load_position_fn(Turtle& turtle)
    {
        if (turtle.stack.empty() || turtle.vertices.size() == 0)
        {
            // Do nothing
        }
        else
        {
            // std::cout << "before: " << turtle.state.width << " ; ";
            // turtle.vertices.push_back( {turtle.vertices.back().position, sf::Color::Transparent} );
            turtle.state = turtle.stack.top();
            // std::cout << "after: " << turtle.state.width << std::endl;
            // turtle.vertices.push_back( {sf::Vector2f(turtle.state.position), sf::Color::Transparent} );
            // turtle.iteration_of_vertices.push_back(turtle.iteration_vec.at(turtle.iteration_index));
            // turtle.iteration_of_vertices.push_back(turtle.iteration_vec.at(turtle.iteration_index));

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
