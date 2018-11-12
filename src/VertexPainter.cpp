#include "VertexPainter.h"
#include "procgui.h"
#include "geometry.h"

namespace colors
{
    VertexPainter::VertexPainter()
        : Observable{}
        , Observer<ColorGenerator>{std::make_shared<LinearGradient>(
            LinearGradient::keys({
                    {sf::Color::Red, 0.},
                    {sf::Color::Green, 0.5},
                    {sf::Color::Blue, 1.}}))}
        , Observer<ColorGeneratorBuffer>{std::make_shared<ColorGeneratorBuffer>(Observer<ColorGenerator>::get_target())}
        , generator_buffer_{Observer<ColorGeneratorBuffer>::get_target()}
    {
        Observer<ColorGenerator>::add_callback([this](){notify();});
        Observer<ColorGeneratorBuffer>::add_callback([this](){update_generator();});
    }

    VertexPainter::VertexPainter(const std::shared_ptr<ColorGenerator> gen)
        : Observable{}
        , Observer<ColorGenerator>(gen)
        , Observer<ColorGeneratorBuffer>{std::make_shared<ColorGeneratorBuffer>(gen)}
        , generator_buffer_{Observer<ColorGeneratorBuffer>::get_target()}
    {
        Observer<ColorGenerator>::add_callback([this](){notify();});
        Observer<ColorGeneratorBuffer>::add_callback([this](){update_generator();});
    }

    VertexPainter::VertexPainter(const VertexPainter& other)
        : Observable{}
        , Observer<ColorGenerator>{other.Observer<ColorGenerator>::get_target()->clone()}
        , Observer<ColorGeneratorBuffer>{std::make_shared<ColorGeneratorBuffer>(Observer<ColorGenerator>::get_target())}
        , generator_buffer_{Observer<ColorGeneratorBuffer>::get_target()}
        , angle_{other.angle_}
    {
        Observer<ColorGenerator>::add_callback([this](){notify();});
        Observer<ColorGeneratorBuffer>::add_callback([this](){update_generator();});
    }

    VertexPainter::VertexPainter(VertexPainter&& other)
        : Observable{}
        , Observer<ColorGenerator>{other.Observer<ColorGenerator>::get_target()->clone()}
        , Observer<ColorGeneratorBuffer>{std::make_shared<ColorGeneratorBuffer>(Observer<ColorGenerator>::get_target())}
        , generator_buffer_{Observer<ColorGeneratorBuffer>::get_target()}
        , angle_{other.angle_}
    {
        Observer<ColorGenerator>::add_callback([this](){notify();});
        Observer<ColorGeneratorBuffer>::add_callback([this](){update_generator();});

        other.Observer<ColorGenerator>::set_target(nullptr);
        other.Observer<ColorGeneratorBuffer>::set_target(nullptr);
        
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

        auto tmp_gen = Observer<ColorGenerator>::get_target();
        Observer<ColorGenerator>::set_target(other.Observer<ColorGenerator>::get_target());
        other.Observer<ColorGenerator>::set_target(tmp_gen);
        Observer<ColorGenerator>::add_callback([this](){notify();});
        other.Observer<ColorGenerator>::add_callback([&other](){other.notify();});

        auto tmp_buff = Observer<ColorGeneratorBuffer>::get_target();
        Observer<ColorGeneratorBuffer>::set_target(other.Observer<ColorGeneratorBuffer>::get_target());
        other.Observer<ColorGeneratorBuffer>::set_target(tmp_buff);
        Observer<ColorGeneratorBuffer>::add_callback([this](){update_generator();});
        other.Observer<ColorGeneratorBuffer>::add_callback([&other](){other.update_generator();});

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


    // const std::shared_ptr<ColorGenerator> VertexPainter::get_generator() const
    // {
    //     return generator_;
    // }

    std::shared_ptr<ColorGeneratorBuffer> VertexPainter::get_generator_buffer() const
    {
        return generator_buffer_;
    }
    
    // void VertexPainter::set_generator(std::shared_ptr<ColorGenerator> generator)
    // {
    //     generator_ = generator;
    //     set_target(generator_);
    //     add_callback([this]{notify();});
    //     notify();
    // }

    void VertexPainter::update_generator()
    {
        auto gen = generator_buffer_->get_generator();
        Observer<ColorGenerator>::set_target(gen);
        Observer<ColorGenerator>::add_callback([this](){notify();});
        notify();
    }

    void VertexPainter::paint_vertices(std::vector<sf::Vertex>& vertices, sf::FloatRect bounding_box) const
    {
        auto generator = Observer<ColorGenerator>::get_target();
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
