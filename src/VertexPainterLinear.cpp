#include <array>
#include "geometry.h"
#include "helper_math.h"
#include "VertexPainterLinear.h"

namespace colors
{
    VertexPainterLinear::VertexPainterLinear()
        : VertexPainter{}
        , angle_{0}
        , center_{0.5,0.5}
    {
    }

    VertexPainterLinear::VertexPainterLinear(const std::shared_ptr<ColorGeneratorWrapper> wrapper)
        : VertexPainter{wrapper}
        , angle_{0}
        , center_{0.5,0.5}
    {
    }
    std::shared_ptr<VertexPainter> VertexPainterLinear::clone() const
    {
        auto clone = std::make_shared<VertexPainterLinear>();
        clone->angle_ = angle_;
        clone->center_ = center_;
        clone->set_target(std::make_shared<ColorGeneratorWrapper>(*get_target()));
        return clone;
    }
    
    float VertexPainterLinear::get_angle() const
    {
        return angle_;
    }
    sf::Vector2f VertexPainterLinear::get_center() const
    {
        return center_;
    }
    
    void VertexPainterLinear::set_angle(float angle)
    {
        angle_ = angle;
        notify();
    }
    void VertexPainterLinear::set_center(sf::Vector2f center)
    {
        center_ = center;
        notify();
    }


    void VertexPainterLinear::paint_vertices(std::vector<sf::Vertex>& vertices,
                                             const std::vector<int>&,
                                             int,
                                             sf::FloatRect bounding_box)
    {
        // // DEBUG
        // static bool first_debug = true;
        
        auto generator = get_target()->unwrap();
        if (!generator)
        {
            return;
        }

        sf::Vector2f direction = {std::cos(math::degree_to_rad(angle_)), -std::sin(math::degree_to_rad(angle_))};
        // sf::Vector2f middle = {bounding_box.left + bounding_box.width/2, bounding_box.top + bounding_box.height/2};
        // Get center coordinates relative to the 'center_'.
        sf::Vector2f relative_center {bounding_box.left + bounding_box.width * center_.x,
                                      bounding_box.top + bounding_box.height * (1.f-center_.y)};
        const auto axis_intersections = geometry::intersection_with_bounding_box({relative_center, direction}, bounding_box);
        
        sf::Vector2f axis_intersection = axis_intersections.first;
        sf::Vector2f axis_opposite_intersection = axis_intersections.second;

        sf::Vector2f upleft = {bounding_box.left, bounding_box.top};
        sf::Vector2f upright = {bounding_box.left+bounding_box.width, bounding_box.top};
        sf::Vector2f downleft= {bounding_box.left, bounding_box.top+bounding_box.height};
        sf::Vector2f downright= {bounding_box.left+bounding_box.width, bounding_box.top+bounding_box.height};
        // std::array<sf::Vector2f, 4> corners {upleft, upright, downleft, downright};
        
        
        float normal_angle = angle_+90;
        sf::Vector2f normal_direction = {std::cos(math::degree_to_rad(normal_angle)), -std::sin(math::degree_to_rad(normal_angle))};
        const auto intersection_upleft = geometry::intersection({relative_center, direction}, {upleft, normal_direction});
        const auto intersection_upright = geometry::intersection({relative_center, direction}, {upright, normal_direction});
        const auto intersection_downleft = geometry::intersection({relative_center, direction}, {downleft, normal_direction});        
        const auto intersection_downright = geometry::intersection({relative_center, direction}, {downright, normal_direction});
        // std::array<sf::Vector2f, 4> intersections {intersection_upleft, intersection_upright, intersection_downleft, intersection_downright};

        float distance_upleft = geometry::distance(upleft, intersection_upleft);
        float distance_upright = geometry::distance(upright, intersection_upright);
        float distance_downleft = geometry::distance(downleft, intersection_downleft);
        float distance_downright = geometry::distance(downright, intersection_downright);
        std::array<float, 4> distances {distance_upleft, distance_upright, distance_downleft, distance_downright};
        auto max_distance = std::max_element(begin(distances), end(distances));
        float distance = *max_distance;
        if(distance < std::numeric_limits<float>::epsilon())
        {
            // The vertices are on the same line, the lerp will always be 0, set
            // distance to 1. to avoid division by 0.;
            distance = 1.f;
        }
        
        // auto max_corner = corners.at(std::distance(begin(distances), max_distance));
        // auto max_intersection = intersections.at(std::distance(begin(distances), max_distance));
        for (auto& v : vertices)
        {
            // sf::Vector2f projection = geometry::project_and_clamp(axis_opposite_intersection, axis_intersection, v.position);
            sf::Vector2f projection = geometry::project(axis_opposite_intersection, axis_intersection, v.position);
            float lerp = geometry::distance(projection, v.position) / distance;

            sf::Color color = generator->get(lerp);
            if (v.color != sf::Color::Transparent)
            {
                v.color = color;
            }
        }

        // // DEBUG
    //     if (!first_debug)
    //     {
    //         for(int i=0; i< 20; ++i)
    //         {
    //             vertices.pop_back();
    //         }
    //     }
        
    //     vertices.push_back({vertices.back().position, {0,0,0,0}});

    //     vertices.push_back({{relative_center.x - 5, relative_center.y - 5}, {0,0,0,0}});
    //     vertices.push_back({{relative_center.x - 5, relative_center.y - 5}, sf::Color::Yellow});
    //     vertices.push_back({{relative_center.x + 5, relative_center.y - 5}, sf::Color::Yellow});
    //     vertices.push_back({{relative_center.x + 5, relative_center.y + 5}, sf::Color::Yellow});
    //     vertices.push_back({{relative_center.x - 5, relative_center.y + 5}, sf::Color::Yellow});
    //     vertices.push_back({{relative_center.x - 5, relative_center.y - 5}, sf::Color::Yellow});
    //     vertices.push_back({{relative_center.x - 5, relative_center.y - 5}, {0,0,0,0}});

    //     vertices.push_back({{axis_intersection.x - 5, axis_intersection.y - 5}, {0,0,0,0}});
    //     vertices.push_back({{axis_intersection.x - 5, axis_intersection.y - 5}, sf::Color::Magenta});
    //     vertices.push_back({{axis_intersection.x + 5, axis_intersection.y - 5}, sf::Color::Magenta});
    //     vertices.push_back({{axis_intersection.x + 5, axis_intersection.y + 5}, sf::Color::Magenta});
    //     vertices.push_back({{axis_intersection.x - 5, axis_intersection.y + 5}, sf::Color::Magenta});
    //     vertices.push_back({{axis_intersection.x - 5, axis_intersection.y - 5}, sf::Color::Magenta});
    //     vertices.push_back({{axis_intersection.x - 5, axis_intersection.y - 5}, {0,0,0,0}});

    //     vertices.push_back({{axis_opposite_intersection.x - 5, axis_opposite_intersection.y - 5}, {0,0,0,0}});
    //     vertices.push_back({{axis_opposite_intersection.x + 5, axis_opposite_intersection.y - 5}, sf::Color::Cyan});
    //     vertices.push_back({{axis_opposite_intersection.x + 5, axis_opposite_intersection.y + 5}, sf::Color::Cyan});
    //     vertices.push_back({{axis_opposite_intersection.x - 5, axis_opposite_intersection.y + 5}, sf::Color::Cyan});
    //     vertices.push_back({{axis_opposite_intersection.x - 5, axis_opposite_intersection.y - 5}, sf::Color::Cyan});
    //     vertices.push_back({{axis_opposite_intersection.x + 5, axis_opposite_intersection.y - 5}, sf::Color::Cyan});
    //     first_debug = false;
    // }
    }

    std::string VertexPainterLinear::type_name() const
    {
        return "VertexPainterLinear";
    }
}

CEREAL_REGISTER_TYPE_WITH_NAME(colors::VertexPainterLinear, "VertexPainterLinear");
CEREAL_REGISTER_POLYMORPHIC_RELATION(colors::VertexPainter, colors::VertexPainterLinear)
