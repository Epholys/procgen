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
    }

    std::tuple<std::vector<sf::Vertex>, std::vector<int>, int>
        compute_vertices(LSystem& lsys,
                         InterpretationMap& interpretation,
                         const DrawingParameters& parameters)

    {
        const auto [str, rec, max] = lsys.produce(parameters.get_n_iter());
        Turtle turtle (parameters, rec);

        // std::cout << "________________________________________________________________";
        
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

        std::vector<sf::Vertex> vertices (begin(turtle.vertices), end(turtle.vertices));
        std::vector<int> iteration_of_vertices (begin(turtle.iteration_of_vertices), end(turtle.iteration_of_vertices));
        
        Ensures(turtle.vertices.size() == turtle.iteration_of_vertices.size());
        return {vertices, iteration_of_vertices, max};
    }
}
