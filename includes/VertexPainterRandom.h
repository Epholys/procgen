#ifndef VERTEX_PAINTER_RANDOM_H
#define VERTEX_PAINTER_RANDOM_H


#include "VertexPainter.h"

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
        
        void randomize() const;
        
        // Paint 'vertices' according to a random real number.
        // 'bounding_box', 'iteration_of_vertices' and 'max_recursion' are not used.
        virtual void paint_vertices(std::vector<sf::Vertex>& vertices,
                                    const std::vector<int>& iteration_of_vertices,
                                    int max_recursion,
                                    sf::FloatRect bounding_box) const override;

    private:
        // Implements the deep-copy cloning.
        virtual std::shared_ptr<VertexPainter> clone_impl() const override;
    };
}

#endif // VERTEX_PAINTER_RANDOM_H
