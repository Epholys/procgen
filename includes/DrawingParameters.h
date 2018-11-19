#ifndef DRAWING_PARAMETERS_H
#define DRAWING_PARAMETERS_H



#include <cmath>
#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>

#include "cereal/cereal.hpp"

#include "helper_math.h"
#include "Observable.h"

// Main explanation of drawing in Turtle.h
namespace drawing
{
    // This data structure contains all constant informations necessary to
    // interpret the result of a L-system. Each attribute can be freely
    // initialized and modified via getters and setters, there are no invariant.
    // During an interpretation, this structure will not be
    // modified.
    class DrawingParameters : public Observable
    {
    public:
        DrawingParameters() = default;
        DrawingParameters(const sf::Vector2f& starting_position);
        DrawingParameters(const sf::Vector2f& starting_position,
                          double starting_angle,
                          double delta_angle,
                          double step,
                          int n_iter);
        DrawingParameters(const DrawingParameters& params) = default;

        // Getters
        sf::Vector2f get_starting_position() const;
        double get_starting_angle() const;
        double get_delta_angle() const;
        double get_step() const;
        int get_n_iter() const;

        // Setters
        // The starting position is only used when rendering the LSystem, so it
        // does not 'notify()' to avoid re-calculating for nothing.
        void set_starting_position(const sf::Vector2f starting_position); 
        void set_starting_angle(double starting_angle);
        void set_delta_angle(double delta_angle);
        void set_step(double step);
        void set_n_iter(int n_iter);
        
    private:
        // The starting position and angle of the Turtle.
        sf::Vector2f starting_position_ { 0, 0 };
        double starting_angle_ { 0 };

        // When 'turn_left' or 'turn_right' orders are executed, the
        // Turtle pivot at a 'delta_angle' angle (in
        // radian). Initialized at an arbitrary value.
        double delta_angle_ { math::pi / 2 };

        // When 'move_forward' order is executed, the Turtle will move
        // forward 'step' pixels (at default zoom level). Initialized
        // at an arbitrary value. 
        double step_ { 20 };

        // The number of iterations done by the L-system.
        int n_iter_ { 0 };

    private:
        // Serialization
        friend class cereal::access;
        
        template <class Archive>
        void save (Archive& ar, const std::uint32_t) const
            {
                ar(cereal::make_nvp("starting_angle", std::round(math::rad_to_degree(starting_angle_)*1000)/1000),
                   cereal::make_nvp("delta_angle", std::round(math::rad_to_degree(delta_angle_)*1000)/1000),
                   CEREAL_NVP(step_),
                   CEREAL_NVP(n_iter_));
            }
        
        template <class Archive>
        void load (Archive& ar, const std::uint32_t)
            {
                ar(starting_angle_, delta_angle_, step_, n_iter_);
                starting_angle_ = math::degree_to_rad(starting_angle_);
                delta_angle_ = math::degree_to_rad(delta_angle_);
                
            }

    };
    
}

#endif
