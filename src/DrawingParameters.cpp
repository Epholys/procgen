#include "DrawingParameters.h"

namespace drawing
{
    DrawingParameters::DrawingParameters(const ext::sf::Vector2d& starting_position,
                      double starting_angle,
                      double delta_angle,
                      double step,
                      int n_iter)
        : starting_position_ {starting_position}
        , starting_angle_ {starting_angle}
        , delta_angle_ {delta_angle}
        , step_ {step}
        , n_iter_ {n_iter}
    {
    }

    DrawingParameters::DrawingParameters(const ext::sf::Vector2d& starting_position, double step)
        : starting_position_ {starting_position}
        , step_ {step}
    {
    }

    ext::sf::Vector2d DrawingParameters::get_starting_position() const
    {
        return starting_position_;
    }
    double DrawingParameters::get_starting_angle() const
    {
        return starting_angle_;
    }
    double DrawingParameters::get_delta_angle() const
    {
        return delta_angle_;
    }
    double DrawingParameters::get_step() const
    {
        return step_;
    }
    int DrawingParameters::get_n_iter() const
    {
        return n_iter_;
    }

    void DrawingParameters::set_starting_position(const ext::sf::Vector2d starting_position)
    {
        starting_position_ = starting_position;
        // The starting position is only used when rendering the LSystem, so it
        // does not 'notify()' to avoid re-calculating for nothing.
    }
    void DrawingParameters::set_starting_angle(double starting_angle)
    {
        starting_angle_ = starting_angle;
        notify();
    }
    void DrawingParameters::set_delta_angle(double delta_angle)
    {
        delta_angle_ = delta_angle;
        notify();
    }
    void DrawingParameters::set_step(double step)
    {
        step_ = step;
        notify();
    }
    void DrawingParameters::set_n_iter(int n_iter)
    {
        n_iter_ = n_iter;
        notify();
    }
}
