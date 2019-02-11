#ifndef VERTEX_PAINTER_RANDOM_H
#define VERTEX_PAINTER_RANDOM_H


#include "VertexPainter.h"
#include "helper_math.h"

namespace colors
{
    class VertexPainterRandom : public VertexPainter
    {
    public:
        VertexPainterRandom(); // Create a default generator
        explicit VertexPainterRandom(const std::shared_ptr<ColorGenerator> gen);
        // Shallow rule-of-five constructors.
        VertexPainterRandom(const VertexPainterRandom& other);
        VertexPainterRandom(VertexPainterRandom&& other);
        VertexPainterRandom& operator=(const VertexPainterRandom& other);
        VertexPainterRandom& operator=(VertexPainterRandom&& other);
        
        // Generate a new seed.
        void randomize();

        // Getter/Setter
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

        // The number of consecutive vertices to paint the same color.
        int block_size_;
        // The seed for 'random_generator_'.
        std::mt19937::result_type random_seed_;
        std::mt19937 random_generator_;
    };
}

#endif // VERTEX_PAINTER_RANDOM_H
