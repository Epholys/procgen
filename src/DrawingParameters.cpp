#include "DrawingParameters.h"

namespace drawing
{
    DrawingParameters::DrawingParameters(const sf::Vector2f& starting_position,
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

    DrawingParameters::DrawingParameters(const sf::Vector2f& starting_position)
        : starting_position_ {starting_position}
    {
    }

    
    // DrawingParameters::DrawingParameters(const DrawingParameters& params)
    //     : starting_position_ {params.starting_position_}
    //     , starting_angle_ {params.starting_angle_}
    //     , delta_angle_ {params.delta_angle_}
    //     , step_ {params.step_}
    //     , n_iter_ {params.n_iter_}
    // {
    // }


    sf::Vector2f DrawingParameters::get_starting_position() const
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

    void DrawingParameters::set_starting_position(const sf::Vector2f starting_position)
    {
        starting_position_ = starting_position;
        notify();
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

    void DrawingParameters::silently_set_starting_position(const sf::Vector2f starting_position)
    {
        starting_position_ = starting_position;
    }

}

