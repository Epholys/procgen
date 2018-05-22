#include "Turtle.h"

namespace drawing
{
    using namespace impl;

    Turtle::Turtle(const DrawingParameters& params)
        : parameters { params }
        , state {{0,0},params.starting_angle}
        , vertices      { { sf::Vector2f(state.position) } }
    {
    }

    std::vector<sf::Vertex> compute_vertices(LSystem& lsys,
                                             InterpretationMap& interpretation,
                                             const DrawingParameters& parameters)
    {
        Turtle turtle (parameters);
        
        const auto res = lsys.produce(parameters.n_iter);

        for (auto c : res)
        {
            if (interpretation.has_predecessor(c))
            {
                // If an interpretation of the character 'c' is found,
                // applies it to the current turtle.
                interpretation.get_rule(c).second(turtle);
            }
            else
            {
                // Do nothing: if 'c' does not have an associated
                // order, it has no effects.
            }
        }

        return turtle.vertices;
    }
}
