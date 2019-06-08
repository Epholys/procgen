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

    VertexPainterRandom::VertexPainterRandom(const std::shared_ptr<ColorGeneratorWrapper> wrapper)
        : VertexPainter{wrapper}
        , block_size_{1}
        , random_seed_(math::random_dev())
        , random_generator_(random_seed_)
    {
    }
    
    std::shared_ptr<VertexPainter> VertexPainterRandom::clone() const
    {
        auto clone = std::make_shared<VertexPainterRandom>();
        clone->block_size_ = block_size_;
        clone->random_seed_ = random_seed_;
        clone->random_generator_ = random_generator_;
        clone->set_target(std::make_shared<ColorGeneratorWrapper>(*get_target()));
        return clone;
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

    std::string VertexPainterRandom::type_name() const
    {
        return "VertexPainterRandom";
    }
}

CEREAL_REGISTER_TYPE_WITH_NAME(colors::VertexPainterRandom, "VertexPainterRandom");
CEREAL_REGISTER_POLYMORPHIC_RELATION(colors::VertexPainter, colors::VertexPainterRandom)
