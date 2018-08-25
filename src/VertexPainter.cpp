#include "VertexPainter.h"
#include "procgui.h"
#include "geometry.h"

namespace colors
{
    VertexPainter::VertexPainter()
        : generator_{nullptr}
    {
        generator_ = std::make_shared<LinearGradient>(LinearGradient::keys({{sf::Color::Red, 0.}, {sf::Color::Green, 0.75}, {sf::Color::Blue, 1.}}));
    }
    VertexPainter::VertexPainter(std::shared_ptr<ColorGenerator> gen)
        : generator_{gen}
    {
    }
    
    void VertexPainter::paint_vertices(std::vector<sf::Vertex>& vertices, sf::FloatRect bounding_box)
    {
        if (!generator_)
        {
            return;
        }

        float angle = 60.f;
        sf::Vector2f direction = {std::cos(math::degree_to_rad(angle)), -std::sin(math::degree_to_rad(angle))};
        sf::Vector2f middle = {bounding_box.left + bounding_box.width/2, bounding_box.top + bounding_box.height/2};
        const auto intersections = geometry::intersection_with_bounding_box({middle, direction}, bounding_box);
        
        sf::Vector2f intersection = intersections.first;
        sf::Vector2f opposite_intersection = intersections.second;
        
        float distance = geometry::distance(opposite_intersection, intersection);
        
        for (auto& v : vertices)
        {
            sf::Vector2f projection = geometry::projection(opposite_intersection, intersection, v.position);
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
        if(procgui::interact_with(generator_, ""))
        {
            notify();
        }
    }
}
