#include "geometry.h"
#include "helper_math.h"
#include "VertexPainterBlockRandom.h"

namespace colors
{
    VertexPainterBlockRandom::VertexPainterBlockRandom()
        : VertexPainter{}
        , block_size_{1}
    {
    }

    VertexPainterBlockRandom::VertexPainterBlockRandom(const std::shared_ptr<ColorGenerator> gen)
        : VertexPainter{gen}
        , block_size_{1}
    {
    }
    
    VertexPainterBlockRandom::VertexPainterBlockRandom(const VertexPainterBlockRandom& other)
        : VertexPainter{other}
        , block_size_{other.block_size_}
    {
    }

    VertexPainterBlockRandom::VertexPainterBlockRandom(VertexPainterBlockRandom&& other)
        : VertexPainter{std::move(other)}
        , block_size_{other.block_size_}
    {
    }

    VertexPainterBlockRandom& VertexPainterBlockRandom::operator=(const VertexPainterBlockRandom& other)
    {
        if (this != &other)
        {
            VertexPainter::operator=(other);
            block_size_ = other.block_size_;
        }
        return *this;
    }

    VertexPainterBlockRandom& VertexPainterBlockRandom::operator=(VertexPainterBlockRandom&& other)
    {
        if (this != &other)
        {
            VertexPainter::operator=(other);
            block_size_ = other.block_size_;
        }
        return *this;
    }

    std::shared_ptr<VertexPainter> VertexPainterBlockRandom::clone_impl() const
    {
        return std::make_shared<VertexPainterBlockRandom>(get_target()->get_generator()->clone());
    }
    
    int VertexPainterBlockRandom::get_block_size() const
    {
        return block_size_;
    }
        
    
    void VertexPainterBlockRandom::set_block_size(int block_size)
    {
        block_size_ = block_size;
        notify();
    }

    
    void VertexPainterBlockRandom::paint_vertices(std::vector<sf::Vertex>& vertices,
                                             const std::vector<int>&,
                                             int,
                                             sf::FloatRect)

    {
        auto generator = get_target()->get_generator();
        if (!generator)
        {
            return;
        }

        int block_index = 0;
        sf::Color color {sf::Color::White};
        for (auto& v : vertices)
        {
            if (block_index % block_size_ == 0)
            {
                float rand = math::random_real(0, 1);
                color = generator->get(rand);
            }
            color.a = v.color.a;
            v.color = color;
            ++block_index;
        }
    }
}
