#include "geometry.h"
#include "helper_math.h"
#include "VertexPainterRandom.h"

namespace colors
{
    VertexPainterRandom::VertexPainterRandom()
        : VertexPainter{}
        , block_size_{1}
    {
    }

    VertexPainterRandom::VertexPainterRandom(const std::shared_ptr<ColorGenerator> gen)
        : VertexPainter{gen}
        , block_size_{1}
    {
    }
    
    VertexPainterRandom::VertexPainterRandom(const VertexPainterRandom& other)
        : VertexPainter{other}
        , block_size_{other.block_size_}
    {
    }

    VertexPainterRandom::VertexPainterRandom(VertexPainterRandom&& other)
        : VertexPainter{std::move(other)}
        , block_size_{other.block_size_}
    {
    }

    VertexPainterRandom& VertexPainterRandom::operator=(const VertexPainterRandom& other)
    {
        if (this != &other)
        {
            VertexPainter::operator=(other);
            block_size_ = other.block_size_;
        }
        return *this;
    }

    VertexPainterRandom& VertexPainterRandom::operator=(VertexPainterRandom&& other)
    {
        if (this != &other)
        {
            VertexPainter::operator=(other);
            block_size_ = other.block_size_;
        }
        return *this;
    }

    std::shared_ptr<VertexPainter> VertexPainterRandom::clone_impl() const
    {
        return std::make_shared<VertexPainterRandom>(get_target()->get_generator()->clone());
    }
    
    int VertexPainterRandom::get_block_size() const
    {
        return block_size_;
    }
        
    
    void VertexPainterRandom::set_block_size(int block_size)
    {
        block_size_ = block_size;
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

        // TODO: document non-opt.
        int block_index = 0;
        float rand = 0;
        for (auto& v : vertices)
        {
            if (block_index % block_size_ == 0)
            {
                rand = math::random_real(0, 1);
            }
            sf::Color color = generator->get(rand);
            color.a = v.color.a;
            v.color = color;
            ++block_index;
        }
    }
}
