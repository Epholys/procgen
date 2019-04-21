#include "geometry.h"
#include "VertexPainterRadial.h"

namespace colors
{
    VertexPainterRadial::VertexPainterRadial()
        : VertexPainter{}
        , center_{.5,.5}
    {
    }

    VertexPainterRadial::VertexPainterRadial(const std::shared_ptr<ColorGeneratorWrapper> wrapper)
        : VertexPainter{wrapper}
        , center_{.5,.5}
    {
    }
    
    // VertexPainterRadial::VertexPainterRadial(const VertexPainterRadial& other)
    //     : VertexPainter{other}
    //     , center_ {other.center_}
    // {
    // }

    // VertexPainterRadial::VertexPainterRadial(VertexPainterRadial&& other)
    //     : VertexPainter{std::move(other)}
    //     , center_ {other.center_}
    // {
    // }

    // VertexPainterRadial& VertexPainterRadial::operator=(const VertexPainterRadial& other)
    // {
    //     if (this != &other)
    //     {
    //         VertexPainter::operator=(other);
    //         center_ = other.center_;
    //     }
    //     return *this;
    // }

    // VertexPainterRadial& VertexPainterRadial::operator=(VertexPainterRadial&& other)
    // {
    //     if (this != &other)
    //     {
    //         VertexPainter::operator=(other);
    //         center_ = other.center_;
    //     }
    //     return *this;
    // }

    std::shared_ptr<VertexPainter> VertexPainterRadial::clone_impl() const
    {
        auto clone = std::make_shared<VertexPainterRadial>();
        clone->center_ = center_;
        clone->set_target(std::make_shared<ColorGeneratorWrapper>(*get_target()));
        return clone;
    }
    
    sf::Vector2f VertexPainterRadial::get_center() const
    {
        return center_;
    }

    void VertexPainterRadial::set_center(sf::Vector2f center)
    {
        center_ = center;
        notify();
    }

    void VertexPainterRadial::paint_vertices(std::vector<sf::Vertex>& vertices,
                                             const std::vector<int>&, 
                                             int,
                                             sf::FloatRect bounding_box)
    {
        auto generator = get_target()->unwrap();
        if (!generator)
        {
            return;
        }

        // Get center coordinates relative to the 'center_'.
        sf::Vector2f relative_center {bounding_box.left + bounding_box.width * center_.x,
                                      bounding_box.top + bounding_box.height * (1.f-center_.y)};

        // Define the corners.
        std::array<sf::Vector2f, 4> corners
            {{{bounding_box.left, bounding_box.top},
            {bounding_box.left + bounding_box.width, bounding_box.top},
            {bounding_box.left, bounding_box.top + bounding_box.height},
            {bounding_box.left + bounding_box.width, bounding_box.top + bounding_box.height}}};

        // Find the farthest corner and compute the distance.
        std::array<float, 4> distances;
        for (unsigned i = 0; i < 4; ++i)
        {
            distances[i] = geometry::distance(relative_center, corners[i]);
        }

        int index = std::distance(begin(distances), std::max_element(begin(distances), end(distances)));
        auto greatest_distance = distances[index];

        if(greatest_distance < std::numeric_limits<float>::epsilon())
        {
            // Avoid division by 0.
            greatest_distance = 1.f;
        }
        for (auto& v : vertices)
        {
            float lerp = geometry::distance(v.position, relative_center) / greatest_distance;
            sf::Color color = generator->get(lerp);
            if (v.color != sf::Color::Transparent)
            {
                v.color = color;
            }
        }

        // // DEBUG
        // vertices.push_back({vertices.back().position, sf::Color::Transparent});
        // vertices.push_back({{relative_center.x - 5, relative_center.y - 5}, sf::Color::Transparent});
        // vertices.push_back({{relative_center.x - 5, relative_center.y - 5}, sf::Color::Magenta});
        // vertices.push_back({{relative_center.x + 5, relative_center.y - 5}, sf::Color::Magenta});
        // vertices.push_back({{relative_center.x + 5, relative_center.y + 5}, sf::Color::Magenta});
        // vertices.push_back({{relative_center.x - 5, relative_center.y + 5}, sf::Color::Magenta});
        // vertices.push_back({{relative_center.x - 5, relative_center.y - 5}, sf::Color::Magenta});
    }
}
