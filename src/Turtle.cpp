#include "Turtle.h"

namespace drawing
{
    using namespace impl;

    Turtle::Turtle(const DrawingParameters& params,
                   const std::vector<int>& iteration_vec)
        : parameters { params }
        , iteration_vec {iteration_vec}
        , iteration_of_vertices {}
          // The other members are set in header as they all derives from
          // 'parameters' or 'iteration_vec'.
    {
        if (!iteration_vec.empty())
        {
            vertices.push_back(sf::Vector2f(state.position));
            iteration_of_vertices.push_back(iteration_vec.at(iteration_index));
        }
    }

    std::tuple<std::vector<sf::Vertex>, std::vector<int>, int>
        compute_vertices(LSystem& lsys,
                         const InterpretationMap& interpretation,
                         const DrawingParameters& parameters)

    {
        const auto [str, rec, max] = lsys.produce(parameters.get_n_iter());
        Turtle turtle (parameters, rec);
        std::cout << "LSys derivation over\n";

        for (auto c : str)
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
            // Increment the iteration index: each character has an iteration
            // count. Can not be in the orders, otherwise characters without
            // orders will not increment this index.
            ++turtle.iteration_index;
        }

        Ensures(turtle.vertices.size() == turtle.iteration_of_vertices.size());
        return {turtle.vertices, turtle.iteration_of_vertices, max};
    }
}
