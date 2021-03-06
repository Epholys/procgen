#include "VertexPainterLinear.h"

#include "RenderWindow.h"
#include "SupplementaryRendering.h"
#include "geometry.h"
#include "helper_color.h"
#include "helper_math.h"

#include <array>

namespace colors
{
VertexPainterLinear::VertexPainterLinear()
    : angle_ {0}
    , display_helper_ {true}
{
}

VertexPainterLinear::VertexPainterLinear(const ColorGeneratorWrapper& wrapper)
    : VertexPainter {wrapper}
    , angle_ {0}
    , display_helper_ {true}
{
}
std::shared_ptr<VertexPainter> VertexPainterLinear::clone() const
{
    auto clone = std::make_shared<VertexPainterLinear>();
    clone->angle_ = angle_;
    clone->generator_ = generator_;
    return clone;
}

float VertexPainterLinear::get_angle() const
{
    return angle_;
}
bool VertexPainterLinear::get_display_flag() const
{
    return display_helper_;
}

void VertexPainterLinear::set_angle(float angle)
{
    angle_ = angle;
    indicate_modification();
}
void VertexPainterLinear::set_display_flag(bool flag)
{
    display_helper_ = flag;
}

void VertexPainterLinear::paint_vertices(std::vector<sf::Vertex>& vertices,
                                         const std::vector<u8>& /*iteration_of_vertices*/,
                                         const std::vector<bool>& transparent,
                                         int /*max_recursion*/,
                                         sf::FloatRect bounding_box)
{
    auto generator = generator_.unwrap();

    // Find the two points on the bounding boxes that intersect the axis.
    sf::Vector2f direction = {float(std::cos(math::degree_to_rad(angle_))),
                              float(-std::sin(math::degree_to_rad(angle_)))};
    sf::Vector2f center = {bounding_box.left + bounding_box.width / 2,
                           bounding_box.top + bounding_box.height / 2};
    const auto axis_intersections = geometry::intersection_with_bounding_box({center, direction},
                                                                             bounding_box);
    sf::Vector2f axis_intersection = axis_intersections.first;
    sf::Vector2f axis_opposite_intersection = axis_intersections.second;


    // Creates the two lines between each the lerping factor will be computed.
    float normal_angle = angle_ + 90;
    sf::Vector2f normal_direction = {float(std::cos(math::degree_to_rad(normal_angle))),
                                     float(-std::sin(math::degree_to_rad(normal_angle)))};
    std::pair<sf::Vector2f, sf::Vector2f> intersection_line {axis_intersection,
                                                             axis_intersection + normal_direction};
    std::pair<sf::Vector2f, sf::Vector2f> opposite_intersection_line {axis_opposite_intersection,
                                                                      axis_opposite_intersection
                                                                          + normal_direction};

    float distance = geometry::distance(axis_intersection, axis_opposite_intersection);
    if (distance < std::numeric_limits<float>::epsilon())
    {
        // The vertices are on the same line, the lerp will always be 0, set
        // distance to 1. to avoid division by 0.;
        distance = 1.f;
    }

#ifdef DEBUG_CHECKS
    for (auto i = 0ull; i < vertices.size(); ++i)
    {
        sf::Vector2f projection = geometry::project(opposite_intersection_line.first,
                                                    opposite_intersection_line.second,
                                                    vertices.at(i).position);
        float lerp = geometry::distance(projection, vertices.at(i).position) / distance;

        sf::Color color = generator->get(lerp);
        if (!transparent.at(i))
        {
            vertices.at(i).color = color;
        }
    }
#else
    for (auto i = 0ull; i < vertices.size(); ++i)
    {
        sf::Vector2f projection = geometry::project(opposite_intersection_line.first,
                                                    opposite_intersection_line.second,
                                                    vertices[i].position);
        float lerp = geometry::distance(projection, vertices[i].position) / distance;

        sf::Color color = generator->get(lerp);
        if (!transparent[i])
        {
            vertices[i].color = color;
        }
    }
#endif
}

void VertexPainterLinear::supplementary_drawing(sf::FloatRect bounding_box) const
{
    if (!display_helper_)
    {
        return;
    }

    sf::Vector2f normal_direction = {float(-std::sin(math::degree_to_rad(angle_))),
                                     float(-std::cos(math::degree_to_rad(angle_)))};
    sf::Vector2f center = {bounding_box.left + bounding_box.width / 2,
                           bounding_box.top + bounding_box.height / 2};
    const auto axis_intersections = geometry::intersection_with_bounding_box(
        {center, normal_direction},
        bounding_box);
    sf::Color indicator_color = colors::bw_contrast_color(sfml_window::background_color);

    std::vector<sf::Vertex> indicator = {{axis_intersections.first, indicator_color},
                                         {axis_intersections.second, indicator_color}};

    procgui::SupplementaryRendering::add_draw_call({indicator});
}

std::string VertexPainterLinear::type_name() const
{
    return "VertexPainterLinear";
}
} // namespace colors
