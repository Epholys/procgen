#include "VertexPainter.h"
#include "procgui.h"
#include "geometry.h"

namespace colors
{
    VertexPainter::VertexPainter()
        : generator_{
        std::make_shared<LinearGradient>(
            LinearGradient::keys({
                    {sf::Color::Red, 0.},
                    {sf::Color::Green, 0.75},
                    {sf::Color::Blue, 1.}}))},
          angle_{60.f}
    {
    }
    VertexPainter::VertexPainter(const std::shared_ptr<ColorGenerator> gen)
        : generator_{gen}
    {
    }
    
    float VertexPainter::get_angle() const
    {
        return angle_;
    }

    void VertexPainter::set_angle(float angle)
    {
        angle_ = angle;
        notify();
    }


    const std::shared_ptr<ColorGenerator> VertexPainter::get_generator() const
    {
        return generator_;
    }
    
    void VertexPainter::paint_vertices(std::vector<sf::Vertex>& vertices, sf::FloatRect bounding_box) const
    {
        if (!generator_)
        {
            return;
        }

        sf::Vector2f direction = {std::cos(math::degree_to_rad(angle_)), -std::sin(math::degree_to_rad(angle_))};
        sf::Vector2f middle = {bounding_box.left + bounding_box.width/2, bounding_box.top + bounding_box.height/2};
        const auto intersections = geometry::intersection_with_bounding_box({middle, direction}, bounding_box);
        
        sf::Vector2f intersection = intersections.first;
        sf::Vector2f opposite_intersection = intersections.second;
        
        float distance = geometry::distance(opposite_intersection, intersection);
        if(distance < std::numeric_limits<float>::epsilon())
        {
            // The vertices are on the same line, the lerp will always be 0, set
            // distance to 1. to avoid division by 0.;
            distance = 1.f;
        }
        
        for (auto& v : vertices)
        {
            sf::Vector2f projection = geometry::project_and_clamp(opposite_intersection, intersection, v.position);
            float lerp = geometry::distance(projection, opposite_intersection) / distance;

            sf::Color color = generator_->get(lerp);
            color.a = v.color.a;
            v.color = color;
        }

        // // DEBUG
        // vertices.push_back({vertices.back().position, {0,0,0,0}});
        // vertices.push_back({{intersection.x - 5, intersection.y - 5}, {0,0,0,0}});
        // vertices.push_back({{intersection.x - 5, intersection.y - 5}, sf::Color::Magenta});
        // vertices.push_back({{intersection.x + 5, intersection.y - 5}, sf::Color::Magenta});
        // vertices.push_back({{intersection.x + 5, intersection.y + 5}, sf::Color::Magenta});
        // vertices.push_back({{intersection.x - 5, intersection.y + 5}, sf::Color::Magenta});
        // vertices.push_back({{intersection.x - 5, intersection.y - 5}, sf::Color::Magenta});
        // vertices.push_back({{intersection.x - 5, intersection.y - 5}, {0,0,0,0}});

        // vertices.push_back({{opposite_intersection.x - 5, opposite_intersection.y - 5}, {0,0,0,0}});
        // vertices.push_back({{opposite_intersection.x + 5, opposite_intersection.y - 5}, sf::Color::Cyan});
        // vertices.push_back({{opposite_intersection.x + 5, opposite_intersection.y + 5}, sf::Color::Cyan});
        // vertices.push_back({{opposite_intersection.x - 5, opposite_intersection.y + 5}, sf::Color::Cyan});
        // vertices.push_back({{opposite_intersection.x - 5, opposite_intersection.y - 5}, sf::Color::Cyan});
        // vertices.push_back({{opposite_intersection.x + 5, opposite_intersection.y - 5}, sf::Color::Cyan});
    }

    void VertexPainter::interact_with()
    {
        if(procgui::interact_with(*this, ""))
        {
            notify(); // TODO: two notify when changing angle: add ColorGenerator to Observer
        }
    }
}
