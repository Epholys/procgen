#include "geometry.h"
#include "helper_math.h"
#include "VertexPainterRandom.h"

namespace colors
{
    VertexPainterRandom::VertexPainterRandom()
        : VertexPainter{}
    {
    }

    VertexPainterRandom::VertexPainterRandom(const std::shared_ptr<ColorGenerator> gen)
        : VertexPainter{gen}
    {
    }
    
    VertexPainterRandom::VertexPainterRandom(const VertexPainterRandom& other)
        : VertexPainter{other}
    {
    }

    VertexPainterRandom::VertexPainterRandom(VertexPainterRandom&& other)
        : VertexPainter{std::move(other)}
    {
    }

    VertexPainterRandom& VertexPainterRandom::operator=(const VertexPainterRandom& other)
    {
        if (this != &other)
        {
            VertexPainter::operator=(other);
        }
        return *this;
    }

    VertexPainterRandom& VertexPainterRandom::operator=(VertexPainterRandom&& other)
    {
        if (this != &other)
        {
            VertexPainter::operator=(other);
        }
        return *this;
    }

    std::shared_ptr<VertexPainter> VertexPainterRandom::clone_impl() const
    {
        return std::make_shared<VertexPainterRandom>(get_target()->get_generator()->clone());
    }
    
    void VertexPainterRandom::randomize() const
    {
        notify();
    }
    
    void VertexPainterRandom::paint_vertices(std::vector<sf::Vertex>& vertices,
                                             const std::vector<int>&,
                                             int,
                                             sf::FloatRect)

    {
        auto generator = get_target()->get_generator();
        if (!generator)
        {
            return;
        }


        for (auto& v : vertices)
        {
            float rand = math::random_real(0, 1);
            sf::Color color = generator->get(rand);
            color.a = v.color.a;
            v.color = color;
        }
    }
}
