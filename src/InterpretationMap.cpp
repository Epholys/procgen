#include "InterpretationMap.h"

#include "Turtle.h"

namespace drawing
{
void go_forward_fn(Turtle& turtle)
{
    // Go forward following the direction vector.
    double dx = turtle.step_ * turtle.state_.direction.x;
    double dy = turtle.step_ * -turtle.state_.direction.y;
    turtle.state_.position += {dx, dy};
    turtle.vertices_.emplace_back(sf::Vector2f(turtle.state_.position));
    turtle.iterations_.push_back(turtle.iteration_depth_);
    turtle.transparency_.push_back(false);
}

void turn_left_fn(Turtle& turtle)
{
    // Updates the direction vector.
    ext::sf::Vector2d v {
        turtle.state_.direction.x * turtle.cos_ - turtle.state_.direction.y * turtle.sin_,
        turtle.state_.direction.x * turtle.sin_ + turtle.state_.direction.y * turtle.cos_};
    turtle.state_.direction = v;
}

void turn_right_fn(Turtle& turtle)
{
    // Updates the direction vector.
    ext::sf::Vector2d v {
        turtle.state_.direction.x * turtle.cos_ - turtle.state_.direction.y * (-turtle.sin_),
        turtle.state_.direction.x * (-turtle.sin_) + turtle.state_.direction.y * turtle.cos_};
    turtle.state_.direction = v;
}

void save_position_fn(Turtle& turtle)
{
    turtle.stack_.push(turtle.state_);
}

void load_position_fn(Turtle& turtle)
{
    if (turtle.stack_.empty() || turtle.vertices_.size() == 0)
    {
        // Do nothing
    }
    else
    {
        turtle.vertices_.emplace_back(turtle.vertices_.back().position, sf::Color::Transparent);
        turtle.state_ = turtle.stack_.top();
        turtle.vertices_.emplace_back(sf::Vector2f(turtle.state_.position), sf::Color::Transparent);
        turtle.vertices_.emplace_back(sf::Vector2f(turtle.state_.position));

        turtle.iterations_.push_back(turtle.iteration_depth_);
        turtle.iterations_.push_back(turtle.iteration_depth_);
        turtle.iterations_.push_back(turtle.iteration_depth_);

        turtle.transparency_.push_back(true);
        turtle.transparency_.push_back(true);
        turtle.transparency_.push_back(false);

        turtle.stack_.pop();
    }
}

InterpretationMap::InterpretationMap(const Rules& rules)
    : RuleMap<Order>(rules)
{
}
InterpretationMap::InterpretationMap(std::initializer_list<typename Rules::value_type> init)
    : RuleMap<Order>(init)
{
}
} // namespace drawing
