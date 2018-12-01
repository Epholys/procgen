#include "geometry.h"
#include "helper_math.h"
#include "VertexPainterRecursion.h"

namespace colors
{
    VertexPainterRecursion::VertexPainterRecursion()
        : VertexPainter{}
    {
    }

    VertexPainterRecursion::VertexPainterRecursion(const std::shared_ptr<ColorGenerator> gen)
        : VertexPainter{gen}
    {
    }
    
    VertexPainterRecursion::VertexPainterRecursion(const VertexPainterRecursion& other)
        : VertexPainter{other}
    {
    }

    VertexPainterRecursion::VertexPainterRecursion(VertexPainterRecursion&& other)
        : VertexPainter{std::move(other)}
    {
    }

    VertexPainterRecursion& VertexPainterRecursion::operator=(const VertexPainterRecursion& other)
    {
        if (this != &other)
        {
            VertexPainter::operator=(other);
        }
        return *this;
    }

    VertexPainterRecursion& VertexPainterRecursion::operator=(VertexPainterRecursion&& other)
    {
        if (this != &other)
        {
            VertexPainter::operator=(other);
        }
        return *this;
    }

    std::shared_ptr<VertexPainter> VertexPainterRecursion::clone_impl() const
    {
        return std::make_shared<VertexPainterRecursion>(get_target()->get_generator()->clone());
    }
    
    void VertexPainterRecursion::paint_vertices(std::vector<sf::Vertex>& vertices,
                                                const std::vector<int>& vertices_recursion,
                                                int max_recursion,
                                                sf::FloatRect) const

    {
        Expects(vertices.size() == vertices_recursion.size());
        
        auto generator = get_target()->get_generator();
        if (!generator)
        {
            return;
        }

        if (max_recursion-1 <= 0)
        {
            // Avoid division by 0.
            max_recursion = 2;
        }


        for (auto i=0u; i<vertices_recursion.size(); ++i)
        {
            sf::Color color = generator->get((vertices_recursion.at(i)-1) / (float(max_recursion)-1));
            sf::Vertex& v = vertices.at(i);
            color.a = v.color.a;
            v.color = color;
        }
    }
}
