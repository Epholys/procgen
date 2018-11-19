#include "VertexPainter.h"
#include "procgui.h"
#include "geometry.h"

namespace colors
{
    VertexPainter::VertexPainter()
        : Observable{}
        , OGen{std::make_shared<ConstantColor>()}
        , OBuff{std::make_shared<ColorGeneratorBuffer>(OGen::get_target())}
        , generator_buffer_{OBuff::get_target()}
    {
        OGen::add_callback([this](){notify();});
        OBuff::add_callback([this](){update_generator();});
    }

    VertexPainter::VertexPainter(const std::shared_ptr<ColorGenerator> gen)
        : Observable{}
        , OGen(gen)
        , OBuff{std::make_shared<ColorGeneratorBuffer>(gen)}
        , generator_buffer_{OBuff::get_target()}
    {
        OGen::add_callback([this](){notify();});
        OBuff::add_callback([this](){update_generator();});
    }

    VertexPainter::VertexPainter(const VertexPainter& other)
        : Observable{}
        , OGen{other.OGen::get_target()->clone()}
        , OBuff{std::make_shared<ColorGeneratorBuffer>(OGen::get_target())}
        , generator_buffer_{OBuff::get_target()}
        , angle_{other.angle_}
    {
        OGen::add_callback([this](){notify();});
        OBuff::add_callback([this](){update_generator();});
    }

    VertexPainter::VertexPainter(VertexPainter&& other)
        : Observable{}
        , OGen{other.OGen::get_target()->clone()}
        , OBuff{std::make_shared<ColorGeneratorBuffer>(OGen::get_target())}
        , generator_buffer_{OBuff::get_target()}
        , angle_{other.angle_}
    {
        OGen::add_callback([this](){notify();});
        OBuff::add_callback([this](){update_generator();});

        other.OGen::set_target(nullptr);
        other.OBuff::set_target(nullptr);
        
        other.angle_ = 0.;
    }

    VertexPainter& VertexPainter::operator=(VertexPainter other)
    {
        swap(other);
        return *this;
    }

    void VertexPainter::swap(VertexPainter& other)
    {
        using std::swap;

        auto tmp_gen = OGen::get_target();
        OGen::set_target(other.OGen::get_target());
        other.OGen::set_target(tmp_gen);
        OGen::add_callback([this](){notify();});
        other.OGen::add_callback([&other](){other.notify();});

        auto tmp_buff = OBuff::get_target();
        OBuff::set_target(other.OBuff::get_target());
        other.OBuff::set_target(tmp_buff);
        OBuff::add_callback([this](){update_generator();});
        other.OBuff::add_callback([&other](){other.update_generator();});

        swap(angle_, other.angle_);
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
        return generator_buffer_;
    }
    
    void VertexPainter::update_generator()
    {
        // Reconcile OGen::ColorGenerator with
        // generator_buffer_::ColorGenerator. 
        auto gen = generator_buffer_->get_generator();
        OGen::set_target(gen);
        OGen::add_callback([this](){notify();});
        notify();
    }

    void VertexPainter::paint_vertices(std::vector<sf::Vertex>& vertices, sf::FloatRect bounding_box) const
    {
        auto generator = OGen::get_target();
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
