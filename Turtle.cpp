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

    std::vector<std::vector<sf::Vertex>> compute_path(LSystem& lsys,
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
}
