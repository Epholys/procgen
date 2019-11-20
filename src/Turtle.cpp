#include "Turtle.h"

namespace drawing
{
    using namespace impl;

    void Turtle::init_from_parameters (const DrawingParameters& parameters)
    {
        cos = std::cos(parameters.get_delta_angle());
        sin = std::sin(parameters.get_delta_angle());
        step = parameters.get_step();
        state = {{0,0},
                 {std::cos(parameters.get_starting_angle()),
                  std::sin(parameters.get_starting_angle())}};
    }

    Turtle::Turtle(const DrawingParameters& parameters)
    {
        init_from_parameters(parameters);
    }

    Turtle::TurtleProduction Turtle::compute_vertices(const std::string& lsystem_production,
                                                      const std::vector<std::uint8_t>& iterations,
                                                      const DrawingParameters& parameters,
                                                      const InterpretationMap& interpretation)
    {
        init_from_parameters(parameters);
        vertices.clear();
        iteration_of_vertices.clear();
        iteration_index = 0;
        iteration = 1;

        if (!iterations.empty())
        {
            vertices.push_back(sf::Vector2f(state.position));
            iteration_of_vertices.push_back(iterations.at(0));
            iteration = iterations.at(0);
        }

        for (auto c : lsystem_production)
        {
            if (interpretation.has_predecessor(c))
            {
                // If an interpretation of the character 'c' is found,
                // applies it to the current turtle.
                interpretation.get_rule(c).second(*this);
            }
            else
            {
                // Do nothing: if 'c' does not have an associated
                // order, it has no effects.
            }
            // Increment the iteration index: each character has an iteration
            // count. Can not be in the orders, otherwise characters without
            // orders will not increment this index. // TODO COMMENT
            iteration = iterations.at(iteration_index++);
        }

        Ensures(vertices.size() == iteration_of_vertices.size());
        TurtleProduction production {vertices, iteration_of_vertices};
        return production;
    }
}
