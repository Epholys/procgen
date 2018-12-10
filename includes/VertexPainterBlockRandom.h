#ifndef VERTEX_PAINTER_BLOCK_RANDOM_H
#define VERTEX_PAINTER_BLOCK_RANDOM_H


#include "VertexPainter.h"

namespace colors
{
    class VertexPainterBlockRandom : public VertexPainter
    {
    public:
        VertexPainterBlockRandom(); // Create a default generator
        explicit VertexPainterBlockRandom(const std::shared_ptr<ColorGenerator> gen);
        // Shallow rule-of-five constructors.
        VertexPainterBlockRandom(const VertexPainterBlockRandom& other);
        VertexPainterBlockRandom(VertexPainterBlockRandom&& other);
        VertexPainterBlockRandom& operator=(const VertexPainterBlockRandom& other);
        VertexPainterBlockRandom& operator=(VertexPainterBlockRandom&& other);
        
        int get_block_size() const;
        void set_block_size(int block_size);

        // Paint 'vertices' according to a random real number.
        // 'bounding_box', 'iteration_of_vertices' and 'max_recursion' are not used.
        virtual void paint_vertices(std::vector<sf::Vertex>& vertices,
                                    const std::vector<int>& iteration_of_vertices,
                                    int max_recursion,
                                    sf::FloatRect bounding_box) override;

    private:
        // Implements the deep-copy cloning.
        virtual std::shared_ptr<VertexPainter> clone_impl() const override;

        int block_size_;
    };
}

#endif // VERTEX_PAINTER_BLOCK_RANDOM_H
