#ifndef DRAWING_PARAMETERS_H
#define DRAWING_PARAMETERS_H


#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>

#include "cereal/cereal.hpp"

#include "helper_math.h"
#include "Observable.h"
#include "WindowController.h"

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
        virtual ~DrawingParameters() = default;
        // Special-case constructor when creating a default LSystem
        DrawingParameters(const ext::sf::Vector2d& starting_position, double step);
        DrawingParameters(const ext::sf::Vector2d& starting_position,
                          double starting_angle,
                          double delta_angle,
                          double step,
                          int n_iter);
        DrawingParameters(const DrawingParameters& params) = default;
        DrawingParameters(DrawingParameters&& params) = default;
        DrawingParameters& operator=(const DrawingParameters& params) = default;
        DrawingParameters& operator=(DrawingParameters&& params) = default;

        // Getters
        ext::sf::Vector2d get_starting_position() const;
        double get_starting_angle() const;
        double get_delta_angle() const;
        double get_step() const;
        int get_n_iter() const;

        // Setters
        // The starting position is only used when rendering the LSystem, so it
        // does not 'notify()' to avoid re-calculating for nothing.
        void set_starting_position(const ext::sf::Vector2d starting_position); 
        void set_starting_angle(double starting_angle);
        void set_delta_angle(double delta_angle);
        void set_step(double step);
        void set_n_iter(int n_iter);
        
    private:
        // The starting position and angle of the Turtle.
        ext::sf::Vector2d starting_position_ { 0, 0 };
        double starting_angle_ { 0 };

        // When 'turn_left' or 'turn_right' orders are executed, the
        // Turtle pivot at a 'delta_angle' angle (in
        // radian). Initialized at an arbitrary value.
        double delta_angle_ {math::pi / 2 };

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
                ar(cereal::make_nvp("starting_angle", math::rad_to_degree(starting_angle_)),
                   cereal::make_nvp("delta_angle", math::rad_to_degree(delta_angle_)),
                   cereal::make_nvp("n_iter", n_iter_));
            }
        
        template <class Archive>
        void load (Archive& ar, const std::uint32_t)
            {
                ar(starting_angle_, delta_angle_, n_iter_);
                if (starting_angle_ < 0 || starting_angle_ > 360)
                {
                    starting_angle_ = math::clamp_angle(starting_angle_);
                    controller::WindowController::add_loading_error_message("DrawingParameters' starting_angle wasn't in the [0,360] range, so it is clamped.");
                }
                if (delta_angle_ < 0 || delta_angle_ > 360)
                {
                    delta_angle_ = math::clamp_angle(delta_angle_);
                    controller::WindowController::add_loading_error_message("DrawingParameters' delta_angle wasn't in the [0,360] range, so it is clamped.");
                }
                if (n_iter_ < 0)
                {
                    n_iter_ = 0;
                    controller::WindowController::add_loading_error_message("DrawingParameters' n_iter was negative, so it is now set to 0.");
                }
                starting_angle_ = math::degree_to_rad(starting_angle_);
                delta_angle_ = math::degree_to_rad(delta_angle_);
            }
    };
}

#endif
