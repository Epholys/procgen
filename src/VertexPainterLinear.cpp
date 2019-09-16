#include "VertexPainterLinear.h"
#include <array>
#include "geometry.h"
#include "helper_math.h"
#include "helper_color.h"
#include "RenderWindow.h"
#include "SupplementaryRendering.h"

namespace colors
{
    VertexPainterLinear::VertexPainterLinear()
        : VertexPainter{}
        , angle_{0}
    {
    }

    VertexPainterLinear::VertexPainterLinear(const std::shared_ptr<ColorGeneratorWrapper> wrapper)
        : VertexPainter{wrapper}
        , angle_{0}
    {
    }
    std::shared_ptr<VertexPainter> VertexPainterLinear::clone() const
    {
        auto clone = std::make_shared<VertexPainterLinear>();
        clone->angle_ = angle_;
        clone->set_target(std::make_shared<ColorGeneratorWrapper>(*get_target()));
        return clone;
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
                                             int,
                                             sf::FloatRect bounding_box)
    {
        auto generator = get_target()->unwrap();
        if (!generator)
        {
            return;
        }

        // Find the two points on the bounding boxes that intersect the axis.
        sf::Vector2f direction = {std::cos(math::degree_to_rad(angle_)), -std::sin(math::degree_to_rad(angle_))};
        sf::Vector2f center = {bounding_box.left + bounding_box.width/2,
                               bounding_box.top + bounding_box.height/2}; 
       const auto axis_intersections = geometry::intersection_with_bounding_box({center, direction}, bounding_box);
        sf::Vector2f axis_intersection = axis_intersections.first;
        sf::Vector2f axis_opposite_intersection = axis_intersections.second;

        
        // Creates the two lines between each the lerping factor will be computed.
        float normal_angle = angle_+90;
        sf::Vector2f normal_direction = {std::cos(math::degree_to_rad(normal_angle)), -std::sin(math::degree_to_rad(normal_angle))};
        std::pair<sf::Vector2f, sf::Vector2f> intersection_line {axis_intersection, axis_intersection+normal_direction};
        std::pair<sf::Vector2f, sf::Vector2f> opposite_intersection_line {axis_opposite_intersection, axis_opposite_intersection+normal_direction};

        float distance = geometry::distance(axis_intersection, axis_opposite_intersection);
        if(distance < std::numeric_limits<float>::epsilon())
        {
            // The vertices are on the same line, the lerp will always be 0, set
            // distance to 1. to avoid division by 0.;
            distance = 1.f;
        }

        for (auto& v : vertices)
        {
            sf::Vector2f projection = geometry::project(opposite_intersection_line.first, opposite_intersection_line.second, v.position);
            float lerp = geometry::distance(projection, v.position) / distance;

            sf::Color color = generator->get(lerp);
            if (v.color != sf::Color::Transparent)
            {
                v.color = color;
            }
        }
    }

    void VertexPainterLinear::supplementary_drawing(sf::FloatRect bounding_box) const
    {
        sf::Vector2f normal_direction = {-std::sin(math::degree_to_rad(angle_)), -std::cos(math::degree_to_rad(angle_))};
        sf::Vector2f center = {bounding_box.left + bounding_box.width/2,
                               bounding_box.top + bounding_box.height/2}; 
       const auto axis_intersections = geometry::intersection_with_bounding_box({center, normal_direction}, bounding_box);
       sf::Color indicator_color = colors::bw_contrast_color(sfml_window::background_color);

       std::vector<sf::Vertex> indicator =
           {{axis_intersections.first, indicator_color},
            {axis_intersections.second, indicator_color}};

       procgui::SupplementaryRendering::add_draw_call({indicator});
    }
    
    std::string VertexPainterLinear::type_name() const
    {
        return "VertexPainterLinear";
    }
}
