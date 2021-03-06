#include "VertexPainterSequential.h"

#include <cmath>

namespace colors
{
VertexPainterSequential::VertexPainterSequential()
    : factor_ {1}
{
}

VertexPainterSequential::VertexPainterSequential(const ColorGeneratorWrapper& wrapper)
    : VertexPainter {wrapper}
    , factor_ {1}
{
}

std::shared_ptr<VertexPainter> VertexPainterSequential::clone() const
{
    auto clone = std::make_shared<VertexPainterSequential>();
    clone->factor_ = factor_;
    clone->generator_ = generator_;
    return clone;
}

float VertexPainterSequential::get_factor() const
{
    return factor_;
}

void VertexPainterSequential::set_factor(float factor)
{
    factor_ = factor;
    indicate_modification();
}

void VertexPainterSequential::paint_vertices(std::vector<sf::Vertex>& vertices,
                                             const std::vector<u8>& /*iteration_of_vertices*/,
                                             const std::vector<bool>& transparent,
                                             int /*max_recursion*/,
                                             sf::FloatRect /*bounding_box*/)

{
    auto generator = generator_.unwrap();

    auto size = vertices.size();
    for (auto i = 0ull; i < vertices.size(); ++i)
    {
        double integral;
        double lerp = std::modf((i * factor_) / size, &integral);
        sf::Color color = generator->get(lerp);
#ifdef DEBUG_CHECKS
        if (!transparent.at(i))
        {
            vertices.at(i).color = color;
        }
#else
        if (!transparent[i])
        {
            vertices[i].color = color;
        }
#endif
    }
}

std::string VertexPainterSequential::type_name() const
{
    return "VertexPainterSequential";
}
} // namespace colors
