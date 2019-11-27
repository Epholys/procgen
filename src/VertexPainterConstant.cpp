#include "VertexPainterConstant.h"

namespace colors
{
    VertexPainterConstant::VertexPainterConstant()
        : VertexPainter{}
    {
    }

    VertexPainterConstant::VertexPainterConstant(const std::shared_ptr<ColorGeneratorWrapper> wrapper)
        : VertexPainter{wrapper}
    {
    }

    std::shared_ptr<VertexPainter> VertexPainterConstant::clone() const
    {
        auto color_wrapper = std::make_shared<ColorGeneratorWrapper>(*get_target());
        return std::make_shared<VertexPainterConstant>(color_wrapper);
    }

    void VertexPainterConstant::paint_vertices(std::vector<sf::Vertex>& vertices,
                                               const std::vector<std::uint8_t>&,
                                               const std::vector<bool>& transparent,
                                               int,
                                               sf::FloatRect)

    {
        auto generator = get_target()->unwrap();
        if (!generator)
        {
            return;
        }


        for (auto i=0ull; i<vertices.size(); ++i)
        {
            sf::Color color = generator->get(.5);
            if (!transparent[i])
            {
                vertices[i].color = color;
            }
        }
    }

    std::string VertexPainterConstant::type_name() const
    {
        return "VertexPainterConstant";
    }
}
