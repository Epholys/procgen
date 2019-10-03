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

    void pleasedo (OrderID id, Turtle& t)
    {
        switch (id)
        {
        case OrderID::GO_FORWARD:
            go_forward_fn(t);
            break;
        case OrderID::TURN_RIGHT :
            turn_right_fn(t);
            break;
        case OrderID::TURN_LEFT:
            turn_left_fn(t);
            break;
        case OrderID::SAVE_POSITION:
            save_position_fn(t);
            break;
        case OrderID::LOAD_POSITION:
            load_position_fn(t);
            break;
        default:
            break;
        }
    }
    
    std::tuple<const std::vector<sf::Vertex>&, const std::vector<int>&>
    Turtle::compute_vertices(const std::string& str,
                             InterpretationMap& interpretation)

    {
        vertices.clear();
        iteration_of_vertices.clear();
        iteration_index = 0;
        if (!iteration_vec.empty())
        {
            vertices.push_back(sf::Vector2f(state.position));
            iteration_of_vertices.push_back(iteration_vec.at(iteration_index));
        }
        
        vertices.reserve(59000000);
        iteration_of_vertices.reserve(59000000);
        
        for (auto c : str)
        {
            if (interpretation.has_predecessor(c))
            {
                // If an interpretation of the character 'c' is found,
                // applies it to the current turtle.
                pleasedo(interpretation.get_rule(c).second.id, *this);
            } else {
              // Do nothing: if 'c' does not have an associated
              // order, it has no effects.
            }
            // Increment the iteration index: each character has an iteration
            // count. Can not be in the orders, otherwise characters without
            // orders will not increment this index.
            ++iteration_index;
        }

        Ensures(vertices.size() == iteration_of_vertices.size());
        std::cout << vertices.size() << '\n';
        return {vertices, iteration_of_vertices};
    }
} // namespace drawing
