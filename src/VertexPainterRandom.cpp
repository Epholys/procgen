#include "VertexPainterRandom.h"

namespace colors
{
    VertexPainterRandom::VertexPainterRandom()
        : VertexPainter{}
        , block_size_{1}
        , random_seed_(math::random_dev())
        , random_generator_(random_seed_)
    {
    }

    VertexPainterRandom::VertexPainterRandom(const std::shared_ptr<ColorGenerator> gen)
        : VertexPainter{gen}
        , block_size_{1}
        , random_seed_(math::random_dev())
        , random_generator_(random_seed_)
    {
    }
    
    VertexPainterRandom::VertexPainterRandom(const VertexPainterRandom& other)
        : VertexPainter{other}
        , block_size_{other.block_size_}
        , random_seed_{other.random_seed_}
        , random_generator_(random_seed_) 
    {
    }

    VertexPainterRandom::VertexPainterRandom(VertexPainterRandom&& other)
        : VertexPainter{std::move(other)}
        , block_size_{other.block_size_}
        , random_seed_{other.random_seed_}
        , random_generator_(random_seed_) 
    {
    }

    VertexPainterRandom& VertexPainterRandom::operator=(const VertexPainterRandom& other)
    {
        if (this != &other)
        {
            VertexPainter::operator=(other);
            block_size_ = other.block_size_;
            random_seed_ = other.random_seed_;
            random_generator_.seed(random_seed_); 
        }
        return *this;
    }

    VertexPainterRandom& VertexPainterRandom::operator=(VertexPainterRandom&& other)
    {
        if (this != &other)
        {
            VertexPainter::operator=(other);
            random_seed_ = other.random_seed_;
        }
        return *this;
    }

    std::shared_ptr<VertexPainter> VertexPainterRandom::clone_impl() const
    {
        return std::make_shared<VertexPainterRandom>(get_target()->unwrap()->clone());
    }

    void VertexPainterRandom::randomize()
    {
        random_seed_ = math::random_dev();
        random_generator_.seed(random_seed_);
        notify();
    }
    
    int VertexPainterRandom::get_block_size() const
    {
        return block_size_;
    }
        
    
    void VertexPainterRandom::set_block_size(int block_size)
    {
        Expects(block_size > 0);
        block_size_ = block_size;
        notify();
    }

    
    void VertexPainterRandom::paint_vertices(std::vector<sf::Vertex>& vertices,
                                             const std::vector<int>&,
                                             int,
                                             sf::FloatRect)

    {
        auto generator = get_target()->unwrap();
        if (!generator)
        {
            return;
        }

        random_generator_.seed(random_seed_); 

        int block_index = 0;
        float rand = 0;
        for (auto& v : vertices)
        {
            if (block_index % block_size_ == 0)
            {
                rand = math::random_real(random_generator_, 0, 1);
            }
            // We call 'get()' each time because we must interact nicely with
            // 'VertexPainterComposite'. 
            sf::Color color = generator->get(rand);
            if (v.color != sf::Color::Transparent)
            {
                v.color = color;
            }
            ++block_index;
        }
    }
}
