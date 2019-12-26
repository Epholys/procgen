#include "gsl/gsl"
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
        clone->generator_ = std::make_shared<ColorGeneratorWrapper>(generator_);
        return clone;
    }

    void VertexPainterRandom::randomize()
    {
        random_seed_ = math::random_dev();
        random_generator_.seed(random_seed_);
        indicate_modification();
    }

    int VertexPainterRandom::get_block_size() const
    {
        return block_size_;
    }


    void VertexPainterRandom::set_block_size(int block_size)
    {
        Expects(block_size > 0);
        block_size_ = block_size;
        indicate_modification();
    }


    void VertexPainterRandom::paint_vertices(std::vector<sf::Vertex>& vertices,
                                             const std::vector<u8>&,
                                             const std::vector<bool>& transparent,
                                             int,
                                             sf::FloatRect)

    {
        auto generator = generator_->unwrap();
        if (!generator)
        {
            return;
        }

        random_generator_.seed(random_seed_);

        int block_index = 0;
        float rand = 0;
        for (auto i=0ull; i<vertices.size(); ++i)
        {
            if (block_index % block_size_ == 0)
            {
                rand = math::random_real(random_generator_, 0, 1);
            }
            // We call 'get()' each time because we must interact nicely with
            // 'VertexPainterComposite'.
            sf::Color color = generator->get(rand);
#ifdef DEBUG_CHECKS
            if (!transparent.at(i))
            {
                vertices.at(i).color = color;
            }
#else
            if (!transparent[i])
            {
                vertices[i].color = color;
            }
#endif
            ++block_index;
        }
    }

    std::string VertexPainterRandom::type_name() const
    {
        return "VertexPainterRandom";
    }
}
