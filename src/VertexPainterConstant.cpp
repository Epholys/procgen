#include "VertexPainterConstant.h"

namespace colors
{
VertexPainterConstant::VertexPainterConstant(const ColorGeneratorWrapper& wrapper)
    : VertexPainter {wrapper}
{
}

std::shared_ptr<VertexPainter> VertexPainterConstant::clone() const
{
    auto color_wrapper = generator_;
    return std::make_shared<VertexPainterConstant>(color_wrapper);
}

void VertexPainterConstant::paint_vertices(std::vector<sf::Vertex>& vertices,
                                           const std::vector<u8>& /*iteration_of_vertices*/,
                                           const std::vector<bool>& transparent,
                                           int /*max_recursion*/,
                                           sf::FloatRect /*bounding_box*/)

{
    auto generator = generator_.unwrap();

#ifdef DEBUG_CHECKS
    for (auto i = 0ull; i < vertices.size(); ++i)
    {
        sf::Color color = generator->get(.5);
        if (!transparent.at(i))
        {
            vertices.at(i).color = color;
        }
    }
#else
    for (auto i = 0ull; i < vertices.size(); ++i)
    {
        sf::Color color = generator->get(.5);
        if (!transparent[i])
        {
            vertices[i].color = color;
        }
    }
#endif
}

std::string VertexPainterConstant::type_name() const
{
    return "VertexPainterConstant";
}
} // namespace colors
