#include "geometry.h"
#include "helper_math.h"
#include "VertexPainterSequential.h"

namespace colors
{
    VertexPainterSequential::VertexPainterSequential()
        : VertexPainter{}
        , factor_{1}
    {
    }

    VertexPainterSequential::VertexPainterSequential(const std::shared_ptr<ColorGenerator> gen)
        : VertexPainter{gen}
        , factor_{1}
    {
    }
    
    VertexPainterSequential::VertexPainterSequential(const VertexPainterSequential& other)
        : VertexPainter{other}
        , factor_ {other.factor_}
    {
    }

    VertexPainterSequential::VertexPainterSequential(VertexPainterSequential&& other)
        : VertexPainter{std::move(other)}
        , factor_ {other.factor_}
    {
    }

    VertexPainterSequential& VertexPainterSequential::operator=(const VertexPainterSequential& other)
    {
        if (this != &other)
        {
            VertexPainter::operator=(other);
            factor_ = other.factor_;
        }
        return *this;
    }

    VertexPainterSequential& VertexPainterSequential::operator=(VertexPainterSequential&& other)
    {
        if (this != &other)
        {
            VertexPainter::operator=(other);
            factor_ = other.factor_;
        }
        return *this;
    }

    std::shared_ptr<VertexPainter> VertexPainterSequential::clone_impl() const
    {
        return std::make_shared<VertexPainterSequential>(get_target()->get_generator()->clone());
    }
    
    float VertexPainterSequential::get_factor() const
    {
        return factor_;
    }

    void VertexPainterSequential::set_factor(float factor)
    {
        factor_ = factor;
        notify();
    }

    void VertexPainterSequential::paint_vertices(std::vector<sf::Vertex>& vertices,
                                                 const std::vector<int>&,
                                                 int,
                                                 sf::FloatRect)

    {
        auto generator = get_target()->get_generator();
        if (!generator)
        {
            return;
        }

        auto size = vertices.size();
        for (auto i = 0u; i < vertices.size(); ++i)
        {
            float integral;
            float lerp = std::modf((i * factor_) / size, &integral);
            sf::Color color = generator->get(lerp);
            // color.a = vertices[i].color.a;
            vertices[i].color = color;
        }
    }
}
