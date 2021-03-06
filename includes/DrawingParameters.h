#ifndef DRAWING_PARAMETERS_H
#define DRAWING_PARAMETERS_H


#include "Indicator.h"
#include "LoadMenu.h"
#include "cereal/cereal.hpp"
#include "helper_math.h"
#include "types.h"

// Main explanation of drawing in Turtle.h
namespace drawing
{
// This data structure contains all constant informations necessary to
// interpret the result of a L-system. Each attribute can be freely
// initialized and modified via getters and setters. The invariant is the
// modification flag from the 'Indicator' inheritance: it must be set to
// true when modifying some of the members (starting_angle, delta_angle, and
// n_iter).
// During an interpretation, this structure will not be modified.
class DrawingParameters : public Indicator
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
                      u8 n_iter);
    DrawingParameters(const DrawingParameters& params) = default;
    DrawingParameters(DrawingParameters&& params) = default;
    DrawingParameters& operator=(const DrawingParameters& params) = default;
    DrawingParameters& operator=(DrawingParameters&& params) = default;

    // Getters
    ext::sf::Vector2d get_starting_position() const;
    double get_starting_angle() const;
    double get_delta_angle() const;
    double get_step() const;
    u8 get_n_iter() const;

    // Setters
    void set_starting_position(const ext::sf::Vector2d starting_position);
    void set_starting_angle(double starting_angle);
    void set_delta_angle(double delta_angle);
    void set_step(double step);
    void set_n_iter(u8 n_iter);

    // Revert to previous 'n_iter'.
    // Useful when cancelling computation of too big L-Systems.
    void revert();

    // Validate modification of 'n_iter'.
    void validate();

  private:
    // The starting position and angle of the Turtle.
    ext::sf::Vector2d starting_position_ {0, 0};
    double starting_angle_ {0};

    // When 'turn_left' or 'turn_right' orders are executed, the Turtle
    // pivot at a 'delta_angle' angle (in radian).
    // Initialized to an arbitrary value.
    double delta_angle_ {math::pi / 2};

    // When 'move_forward' order is executed, the Turtle will move forward
    // 'step' pixels (at default zoom level).
    // Initialized to an arbitrary value.
    // In reality, this is a scale value: the dimension on screen of the
    // L-System is done by a graphic transform that is very efficient, so
    // 'step_' is simply the zoom level (the transform is scaled as
    // 'DrawingParameters::get_step() / Turtle::step_', which is 1.
    double step_ {10};

    // The number of iterations computed by the L-system.
    u8 n_iter_ {0};

    // The previous number of iterations.
    // Used in 'revert()' and 'validate()'.
    int previous_n_iter_ {-1};

  private:
    // Serialization
    friend class cereal::access;

    template<class Archive>
    void save(Archive& ar, const u32) const
    {
        ar(cereal::make_nvp("starting_angle", math::rad_to_degree(starting_angle_)),
           cereal::make_nvp("delta_angle", math::rad_to_degree(delta_angle_)),
           cereal::make_nvp("n_iter", n_iter_));
    }

    template<class Archive>
    void load(Archive& ar, const u32)
    {
        ar(starting_angle_, delta_angle_, n_iter_);
        if (starting_angle_ < 0 || starting_angle_ > 360)
        {
            starting_angle_ = math::clamp_angle(starting_angle_);
            controller::LoadMenu::add_loading_error_message(
                "DrawingParameters' starting_angle wasn't in the [0,360] range, so was clamped.");
        }
        if (delta_angle_ < 0 || delta_angle_ > 360)
        {
            delta_angle_ = math::clamp_angle(delta_angle_);
            controller::LoadMenu::add_loading_error_message(
                "DrawingParameters' delta_angle wasn't in the [0,360] range, so it was clamped.");
        }
        if (n_iter_ < 0)
        {
            n_iter_ = 0;
            controller::LoadMenu::add_loading_error_message(
                "DrawingParameters' n_iter was negative, so it is now set to 0.");
        }
        starting_angle_ = math::degree_to_rad(starting_angle_);
        delta_angle_ = math::degree_to_rad(delta_angle_);
    }
};
} // namespace drawing

#endif
