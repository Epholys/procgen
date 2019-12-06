#include "Turtle.h"

namespace drawing
{
    // Switch case to apply the order function associated to 'order' to
    // 'turtle'.
    void execute_order(const OrderID& order, Turtle& turtle)
    {
        switch (order)
        {
        case OrderID::GO_FORWARD:
            go_forward_fn(turtle);
            break;
        case OrderID::TURN_RIGHT:
            turn_right_fn(turtle);
            break;
        case OrderID::TURN_LEFT:
            turn_left_fn(turtle);
            break;
        case OrderID::SAVE_POSITION:
            save_position_fn(turtle);
            break;
        case OrderID::LOAD_POSITION:
            load_position_fn(turtle);
            break;
        default:
            Expects(false);
            break;
        }
    }


    void Turtle::init_from_parameters (const DrawingParameters& parameters)
    {
        cos_ = std::cos(parameters.get_delta_angle());
        sin_ = std::sin(parameters.get_delta_angle());
        state_ = {{0,0},
                 {std::cos(parameters.get_starting_angle()),
                  std::sin(parameters.get_starting_angle())}};
    }

    Turtle::Turtle(const DrawingParameters& parameters)
    {
        init_from_parameters(parameters);
    }

    Turtle::TurtleProduction Turtle::compute_vertices(const std::string& lsystem_production,
                                                      const std::vector<u8>& lsystem_iterations,
                                                      const InterpretationMap& interpretation,
                                                      unsigned long long size)
    {
        // Reset the members
        vertices_.clear();
        iterations_.clear();
        transparency_.clear();
        iteration_index_ = 0;
        iteration_depth_ = 0;

        // Reserve memory
        vertices_.reserve(size);
        iterations_.reserve(size);
        transparency_.reserve(size);

        // If there is at least one vertex, create manually the first one at the
        // origin.
        if (!lsystem_production.empty())
        {
            vertices_.push_back(sf::Vector2f(state_.position));
            iterations_.push_back(lsystem_iterations.at(0));
            transparency_.push_back(false);
            iteration_depth_ = iterations_.at(0);
        }

        for (auto c : lsystem_production)
        {
            if (interpretation.has_predecessor(c))
            {
                // If an interpretation of the character 'c' is found,
                // applies it to the current turtle.
                execute_order(interpretation.get_rule(c).second.id, *this);
            }
            else
            {
                // Do nothing: if 'c' does not have an associated
                // order, it has no effects.
            }

            // Update the iteration depth for the vertices of the next symbol.
            // Operation that apply the invariant
            iteration_depth_ = lsystem_iterations.at(iteration_index_++);
        }

        // Ensures the invariant
        Ensures(vertices_.size() == iterations_.size());
        Ensures(vertices_.size() == transparency_.size());
        TurtleProduction production {vertices_, iterations_, transparency_};
        return production;
    }
}
