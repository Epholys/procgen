#include "VertexPainter.h"
#include "procgui.h"
#include "geometry.h"

namespace colors
{
    VertexPainter::VertexPainter()
        : Observable{}
        , OGenBuff{std::make_shared<ColorGeneratorBuffer>()}
    {
        add_callback([this](){notify();});
    }

    VertexPainter::VertexPainter(const std::shared_ptr<ColorGenerator> gen)
        : Observable{}
        , OGenBuff{std::make_shared<ColorGeneratorBuffer>(gen)}
    {
        add_callback([this](){notify();});
    }

    VertexPainter::VertexPainter(const VertexPainter& other)
        : Observable{}
        , OGenBuff{other.get_target()}
    {
        add_callback([this](){notify();});
    }

    VertexPainter::VertexPainter(VertexPainter&& other)
        : Observable{}
        , OGenBuff{std::move(other.get_target())}
        , angle_{other.angle_}
    {
        add_callback([this](){notify();});
        
        other.set_target(nullptr);
    }

    VertexPainter& VertexPainter::operator=(const VertexPainter& other)
    {
        if (this != &other)
        {
            OGenBuff{other.get_target()};
            angle_ = {other.angle_};

            add_callback([this](){notify();});
        }
        return *this;
    }

    VertexPainter& VertexPainter::operator=(VertexPainter&& other)
    {
        if (this != &other)
        {
            OGenBuff {std::move(other.get_target())};
            angle_ = other.angle_;

            add_callback([this](){notify();});
            
            other.set_target(nullptr);
        }
        return *this;
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

    std::shared_ptr<ColorGeneratorBuffer> VertexPainter::get_generator_buffer() const
    {
        return get_target();
    }
    
    void VertexPainter::paint_vertices(std::vector<sf::Vertex>& vertices, sf::FloatRect bounding_box) const
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
