#include "VertexPainterRandom.h"

#include "gsl/gsl"

namespace colors
{
VertexPainterRandom::VertexPainterRandom()
    : block_size_ {1}
    , random_seed_(math::random_dev())
    , random_generator_(random_seed_)
{
}

VertexPainterRandom::VertexPainterRandom(const ColorGeneratorWrapper& wrapper)
    : VertexPainter {wrapper}
    , block_size_ {1}
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
    clone->generator_ = generator_;
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
                                         const std::vector<u8>& /*iteration_of_vertices*/,
                                         const std::vector<bool>& transparent,
                                         int /*max_recursion*/,
                                         sf::FloatRect /*bounding_box*/)

{
    auto generator = generator_.unwrap();

    random_generator_.seed(random_seed_);

    int block_index = 0;
    float rand = 0;
    for (auto i = 0ull; i < vertices.size(); ++i)
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
} // namespace colors
