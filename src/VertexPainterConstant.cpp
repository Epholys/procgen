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
    
    // VertexPainterConstant::VertexPainterConstant(const VertexPainterConstant& other)
    //     : VertexPainter{other}
    // {
    // }

    // VertexPainterConstant::VertexPainterConstant(VertexPainterConstant&& other)
    //     : VertexPainter{std::move(other)}
    // {
    // }

    // VertexPainterConstant& VertexPainterConstant::operator=(const VertexPainterConstant& other)
    // {
    //     if (this != &other)
    //     {
    //         VertexPainter::operator=(other);
    //     }
    //     return *this;
    // }

    // VertexPainterConstant& VertexPainterConstant::operator=(VertexPainterConstant&& other)
    // {
    //     if (this != &other)
    //     {
    //         VertexPainter::operator=(other);
    //     }
    //     return *this;
    // }

    std::shared_ptr<VertexPainter> VertexPainterConstant::clone_impl() const
    {
        auto color_wrapper = std::make_shared<ColorGeneratorWrapper>(*get_target());
        return std::make_shared<VertexPainterConstant>(color_wrapper);
    }
    
    void VertexPainterConstant::paint_vertices(std::vector<sf::Vertex>& vertices,
                                             const std::vector<int>&,
                                             int,
                                             sf::FloatRect)

    {
        auto generator = get_target()->unwrap();
        if (!generator)
        {
            return;
        }


        for (auto& v : vertices)
        {
            sf::Color color = generator->get(.5);
            if (v.color != sf::Color::Transparent)
            {
                v.color = color;
            }
        }
    }
}
