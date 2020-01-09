#include "VertexPainterRadial.h"

#include "RenderWindow.h"
#include "SupplementaryRendering.h"
#include "geometry.h"
#include "helper_color.h"
#include "helper_math.h"

namespace colors
{
VertexPainterRadial::VertexPainterRadial()
    : center_ {.5, .5}
    , display_helper_ {true}
{
}

VertexPainterRadial::VertexPainterRadial(const ColorGeneratorWrapper& wrapper)
    : VertexPainter {wrapper}
    , center_ {.5, .5}
    , display_helper_ {true}
{
}

std::shared_ptr<VertexPainter> VertexPainterRadial::clone() const
{
    auto clone = std::make_shared<VertexPainterRadial>();
    clone->center_ = center_;
    clone->generator_ = generator_;
    return clone;
}

sf::Vector2f VertexPainterRadial::get_center() const
{
    return center_;
}
bool VertexPainterRadial::get_display_flag() const
{
    return display_helper_;
}

void VertexPainterRadial::set_center(sf::Vector2f center)
{
    center_ = center;
    indicate_modification();
}
void VertexPainterRadial::set_display_flag(bool flag)
{
    display_helper_ = flag;
}


void VertexPainterRadial::paint_vertices(std::vector<sf::Vertex>& vertices,
                                         const std::vector<u8>& /*iteration_of_vertices*/,
                                         const std::vector<bool>& transparent,
                                         int /*max_recursion*/,
                                         sf::FloatRect bounding_box)
{
    auto generator = generator_.unwrap();

    // Get center coordinates relative to the 'center_'.
    sf::Vector2f relative_center {bounding_box.left + bounding_box.width * center_.x,
                                  bounding_box.top + bounding_box.height * (1.f - center_.y)};

    // Define the corners.
    std::array<sf::Vector2f, 4> corners {
        {{bounding_box.left, bounding_box.top},
         {bounding_box.left + bounding_box.width, bounding_box.top},
         {bounding_box.left, bounding_box.top + bounding_box.height},
         {bounding_box.left + bounding_box.width, bounding_box.top + bounding_box.height}}};

    // Find the farthest corner and compute the distance.
    std::array<float, 4> distances {};
    for (unsigned i = 0; i < 4; ++i)
    {
        distances[i] = geometry::distance(relative_center, corners[i]);
    }

    int index = std::distance(begin(distances), std::max_element(begin(distances), end(distances)));
    auto greatest_distance = distances[index];

    if (greatest_distance < std::numeric_limits<float>::epsilon())
    {
        // Avoid division by 0.
        greatest_distance = 1.f;
    }


#ifdef DEBUG_CHECKS
    for (auto i = 0ull; i < vertices.size(); ++i)
    {
        float lerp = geometry::distance(vertices.at(i).position, relative_center)
                     / greatest_distance;
        sf::Color color = generator->get(lerp);
        if (!transparent.at(i))
        {
            vertices.at(i).color = color;
        }
    }
#else
    for (auto i = 0ull; i < vertices.size(); ++i)
    {
        float lerp = geometry::distance(vertices[i].position, relative_center) / greatest_distance;
        sf::Color color = generator->get(lerp);
        if (!transparent[i])
        {
            vertices[i].color = color;
        }
    }
#endif
    // // DEBUG
    // vertices.push_back({vertices.back().position, sf::Color::Transparent});
    // vertices.push_back({{relative_center.x - 5, relative_center.y - 5}, sf::Color::Transparent});
    // vertices.push_back({{relative_center.x - 5, relative_center.y - 5}, sf::Color::Magenta});
    // vertices.push_back({{relative_center.x + 5, relative_center.y - 5}, sf::Color::Magenta});
    // vertices.push_back({{relative_center.x + 5, relative_center.y + 5}, sf::Color::Magenta});
    // vertices.push_back({{relative_center.x - 5, relative_center.y + 5}, sf::Color::Magenta});
    // vertices.push_back({{relative_center.x - 5, relative_center.y - 5}, sf::Color::Magenta});
}

void VertexPainterRadial::supplementary_drawing(sf::FloatRect bounding_box) const
{
    if (!display_helper_)
    {
        return;
    }

    constexpr float ratio = 1 / 16.f;
    float half_indicator_size = bounding_box.width < bounding_box.height
                                    ? ratio * bounding_box.width / 2.
                                    : ratio * bounding_box.height / 2.;
    sf::Vector2f center = {bounding_box.left + bounding_box.width * center_.x,
                           bounding_box.top + bounding_box.height * (1 - center_.y)};
    sf::Color indicator_color = colors::bw_contrast_color(sfml_window::background_color);

    auto draw_circle = [](sf::Vector2f center, float radius, int n_point) {
        std::vector<sf::Vertex> circle;
        float angle = 0.f;
        for (int i = 0; i < n_point; ++i)
        {
            circle.push_back({{center.x + radius * (float)std::cos(angle),
                               center.y + radius * (float)std::sin(angle)}});
            angle += 2 * math::pi / n_point;
        }
        circle.push_back(circle.front());
        return circle;
    };

    std::vector<sf::Vertex> circle = draw_circle(center, half_indicator_size, 10);
    for (sf::Vertex& v : circle)
    {
        v.color = indicator_color;
    }

    procgui::SupplementaryRendering::add_draw_call({circle, sf::LineStrip});
}

std::string VertexPainterRadial::type_name() const
{
    return "VertexPainterRadial";
}
} // namespace colors
