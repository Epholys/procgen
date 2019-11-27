#include "VertexPainterIteration.h"

namespace colors
{
    VertexPainterIteration::VertexPainterIteration()
        : VertexPainter{}
    {
    }

    VertexPainterIteration::VertexPainterIteration(const std::shared_ptr<ColorGeneratorWrapper> wrapper)
        : VertexPainter{wrapper}
    {
    }

    std::shared_ptr<VertexPainter> VertexPainterIteration::clone() const
    {
        auto color_wrapper = std::make_shared<ColorGeneratorWrapper>(*get_target());
        return std::make_shared<VertexPainterIteration>(color_wrapper);
    }

    void VertexPainterIteration::paint_vertices(std::vector<sf::Vertex>& vertices,
                                                const std::vector<std::uint8_t>& vertices_iteration,
                                                const std::vector<bool>& transparent,
                                                int max_iteration,
                                                sf::FloatRect)

    {
        Expects(vertices.size() == vertices_iteration.size());

        auto generator = get_target()->unwrap();
        if (!generator)
        {
            return;
        }

        if (max_iteration-1 <= 0)
        {
            // Avoid division by 0.
            max_iteration = 2;
        }


        for (auto i=0u; i<vertices_iteration.size(); ++i)
        {
#ifdef VERTEX_PAINTER_ITERATION_BUGGY
            sf::Color color = generator->get((vertices_iteration.at(i)-1) / (float(max_iteration)-1));
#else
            sf::Color color = generator->get((vertices_iteration.at(i)) / (float(max_iteration)));
#endif
            sf::Vertex& v = vertices[i];
            if (!transparent[i])
            {
                v.color = color;
            }
        }
    }

    std::string VertexPainterIteration::type_name() const
    {
        return "VertexPainterIteration";
    }

}
