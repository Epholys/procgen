#include "VertexPainterIteration.h"

namespace colors
{
    VertexPainterIteration::VertexPainterIteration()
        : VertexPainter{}
    {
    }

    VertexPainterIteration::VertexPainterIteration(const std::shared_ptr<ColorGenerator> gen)
        : VertexPainter{gen}
    {
    }
    
    VertexPainterIteration::VertexPainterIteration(const VertexPainterIteration& other)
        : VertexPainter{other}
    {
    }

    VertexPainterIteration::VertexPainterIteration(VertexPainterIteration&& other)
        : VertexPainter{std::move(other)}
    {
    }

    VertexPainterIteration& VertexPainterIteration::operator=(const VertexPainterIteration& other)
    {
        if (this != &other)
        {
            VertexPainter::operator=(other);
        }
        return *this;
    }

    VertexPainterIteration& VertexPainterIteration::operator=(VertexPainterIteration&& other)
    {
        if (this != &other)
        {
            VertexPainter::operator=(other);
        }
        return *this;
    }

    std::shared_ptr<VertexPainter> VertexPainterIteration::clone_impl() const
    {
        auto clone = std::make_shared<VertexPainterIteration>(*this);
        clone->set_target(std::make_shared<ColorGeneratorWrapper>(get_target()->clone()));
        return clone;
    }
    
    void VertexPainterIteration::paint_vertices(std::vector<sf::Vertex>& vertices,
                                                const std::vector<int>& vertices_iteration,
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
            sf::Color color = generator->get((vertices_iteration.at(i)-1) / (float(max_iteration)-1));
            sf::Vertex& v = vertices.at(i);
            if (v.color != sf::Color::Transparent)
            {
                v.color = color;
            }
        }
    }
}
