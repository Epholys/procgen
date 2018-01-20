#include "Turtle.h"


namespace drawing
{
    using namespace impl;

    Turtle::Turtle(const DrawingParameters& params)
        : parameters { params }
        , position   { parameters.starting_position }
        , angle      { parameters.starting_angle }
        , vertices   { {position} }
    {
    }
        
    
    std::vector<sf::Vertex> compute_vertices(lsys::LSystem& lsys,
                                             interpretation_map& interpretation,
                                             const DrawingParameters& parameters)
    {
        Turtle turtle (parameters);
        
        const auto res = lsys.produce(parameters.n_iter);

        for (auto c : res)
        {
            if (interpretation.count(c) > 0)
            {
                // If an interpretation of the character 'c' is found,
                // applies it to the current turtle.
                interpretation.at(c)(turtle);
            }
            else
            {
                // Do nothing: if 'c' does not have an associated
                // order, it has no effects.
            }
        }

        return turtle.vertices;
    }

    void go_forward_fn(Turtle& turtle)
    {
        float dx = turtle.parameters.step * std::cos(turtle.angle);
        float dy = turtle.parameters.step * std::sin(turtle.angle);
        turtle.position += {dx, dy};
        turtle.vertices.push_back(turtle.position);
    }

    void turn_right_fn(Turtle& turtle)
    {
        turtle.angle += turtle.parameters.delta_angle;
    }

    void turn_left_fn(Turtle& turtle)
    {
        turtle.angle -= turtle.parameters.delta_angle;
    }
}
