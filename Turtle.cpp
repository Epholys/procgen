#include "Turtle.h"

void Turtle::add_order(char c, const std::function<void(Turtle*)>& fn)
{
    orders[c] = std::bind(fn, this);
}
void go_forward(Turtle* turtle)
{
    float dx = turtle->step * std::cos(turtle->curr_angle);
    float dy = turtle->step * std::sin(turtle->curr_angle);
    turtle->curr_pos += {dx, dy};
    turtle->vertices.push_back(turtle->curr_pos);
}
void turn_right(Turtle* turtle)
{
    turtle->curr_angle += turtle->delta_angle;
}

void turn_left(Turtle* turtle)
{
    turtle->curr_angle -= turtle->delta_angle;
}


void Turtle::compute_vertices(int n_iter)
{
    const auto res = lsys.iter(n_iter);
    
    vertices = { sf::Vertex(curr_pos) };

    for (auto c : res) {
        if (orders.count(c) > 0) {
            orders.at(c)();
        }
        else {
            // Do nothing
        }
    }

}

void Turtle::draw(sf::RenderTarget& target, sf::RenderStates states) const
{      
    target.draw(vertices.data(), vertices.size(), sf::LinesStrip, states);
}
