#ifndef DRAWING_PARAMETERS_H
#define DRAWING_PARAMETERS_H



#include <cmath>
#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>

#include "cereal/cereal.hpp"

#include "helper_math.h"
#include "helper_serialization.h"

// Main explanation of drawing in Turtle.h
namespace drawing
{
    // This data structure contains all constant informations
    // necessary to interpret the result of a L-system. Each attribute
    // can be freely initialized and modified, there are no invariant.
    // During an interpretation, this structure will not be
    // modified.
    struct DrawingParameters
    {
        // The starting position and angle of the Turtle.
        sf::Vector2f starting_position { 0, 0 };
        float starting_angle { 0 };

        // When 'turn_left' or 'turn_right' orders are executed, the
        // Turtle pivot at a 'delta_angle' angle (in
        // radian). Initialized at an arbitrary value.
        double delta_angle { math::pi / 2 };

        // When 'move_forward' order is executed, the Turtle will move
        // forward 'step' pixels (at default zoom level). Initialized
        // at an arbitrary value. 
        float step { 20 };

        // The number of iterations done by the L-system.
        int n_iter { 0 };

    private:
        // Serialization
        friend class cereal::access;
        
        template <class Archive>
        void save (Archive& ar, const std::uint32_t) const
            {
                ar(// CEREAL_NVP(starting_position),
                   CEREAL_NVP(starting_angle),
                   cereal::make_nvp("delta_angle", std::round(math::rad_to_degree(delta_angle))),
                   CEREAL_NVP(step),
                   CEREAL_NVP(n_iter));
            }
        
        template <class Archive>
        void load (Archive& ar, const std::uint32_t)
            {
                ar(// starting_position, 
                   starting_angle, delta_angle, step, n_iter);
                delta_angle = math::degree_to_rad(delta_angle);
            }

    };
    
}

#endif
