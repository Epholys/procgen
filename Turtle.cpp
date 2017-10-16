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
        
    
    std::vector<sf::Vertex> compute_vertices(const LSysInterpretation& interpretation,
                                             const DrawingParameters& parameters)
    {
        Turtle turtle (parameters);
        
        lsys::LSystem lsys = interpretation.lsys;
        
        const auto res = lsys.produce(parameters.n_iter);

        for (auto c : res) {
            if (interpretation.map.count(c) > 0) {
                // If an interpretation of the character 'c' is found,
                // applies it to the current turtle.
                interpretation.map.at(c)(turtle);
            }
            else {
                // Do nothing: if 'c' does not have an associated
                // order, it has no effects.
            }
        }

        return turtle.vertices;
    }

    void go_forward(Turtle& turtle)
    {
        float dx = turtle.parameters.step * std::cos(turtle.angle);
        float dy = turtle.parameters.step * std::sin(turtle.angle);
        turtle.position += {dx, dy};
        turtle.vertices.push_back(turtle.position);
    }

    void turn_right(Turtle& turtle)
    {
        turtle.angle += turtle.parameters.delta_angle;
    }

    void turn_left(Turtle& turtle)
    {
        turtle.angle -= turtle.parameters.delta_angle;
    }
}
