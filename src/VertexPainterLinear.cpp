#include "geometry.h"
#include "helper_math.h"
#include "VertexPainterLinear.h"

namespace colors
{
    VertexPainterLinear::VertexPainterLinear()
        : VertexPainter{}
        , angle_{0}
    {
    }

    VertexPainterLinear::VertexPainterLinear(const std::shared_ptr<ColorGenerator> gen)
        : VertexPainter{gen}
        , angle_{0}
    {
    }
    
    VertexPainterLinear::VertexPainterLinear(const VertexPainterLinear& other)
        : VertexPainter{other}
        , angle_ {other.angle_}
    {
    }

    VertexPainterLinear::VertexPainterLinear(VertexPainterLinear&& other)
        : VertexPainter{std::move(other)}
        , angle_ {other.angle_}
    {
    }

    VertexPainterLinear& VertexPainterLinear::operator=(const VertexPainterLinear& other)
    {
        if (this != &other)
        {
            VertexPainter::operator=(other);
            angle_ = other.angle_;
        }
        return *this;
    }

    VertexPainterLinear& VertexPainterLinear::operator=(VertexPainterLinear&& other)
    {
        if (this != &other)
        {
            VertexPainter::operator=(other);
            angle_ = other.angle_;
        }
        return *this;
    }

    std::shared_ptr<VertexPainter> VertexPainterLinear::clone_impl() const
    {
        return std::make_shared<VertexPainterLinear>(get_target()->get_generator()->clone());
    }
    
    float VertexPainterLinear::get_angle() const
    {
        return angle_;
    }

    void VertexPainterLinear::set_angle(float angle)
    {
        angle_ = angle;
        notify();
    }

    void VertexPainterLinear::paint_vertices(std::vector<sf::Vertex>& vertices,
                                             const std::vector<int>&,
                                             sf::FloatRect bounding_box) const
    {
        auto generator = get_target()->get_generator();
        if (!generator)
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

            sf::Color color = generator->get(lerp);
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
}
