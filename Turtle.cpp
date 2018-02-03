#include "Turtle.h"


namespace drawing
{
    using namespace impl;

    Turtle::Turtle(const DrawingParameters& params)
        : parameters { params }
        , state   { parameters.starting_position, parameters.starting_angle }
        , paths      { { state.position } }
    {
    }
        
    
    std::vector<std::vector<sf::Vertex>> compute_path(lsys::LSystem& lsys,
                                                      InterpretationMap& interpretation,
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

        return turtle.paths;
    }

    void go_forward_fn(Turtle& turtle)
    {
        float dx = turtle.parameters.step * std::cos(turtle.state.angle);
        float dy = turtle.parameters.step * std::sin(turtle.state.angle);
        turtle.state.position += {dx, dy};
        turtle.paths.back().push_back(turtle.state.position);
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
            turtle.state.position = turtle.stack.top().position;
            turtle.state.angle = turtle.stack.top().angle;
            turtle.paths.push_back( {turtle.stack.top().position} );
            turtle.stack.pop();
        }
    }

}
