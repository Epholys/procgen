#ifndef VERTEX_PAINTER_SEQUENTIAL_H
#define VERTEX_PAINTER_SEQUENTIAL_H


#include "VertexPainter.h"

namespace colors
{
    class VertexPainterSequential : public VertexPainter
    {
    public:
        VertexPainterSequential(); // Create a default generator
        explicit VertexPainterSequential(const std::shared_ptr<ColorGenerator> gen);
        // Shallow rule-of-five constructors.
        VertexPainterSequential(const VertexPainterSequential& other);
        VertexPainterSequential(VertexPainterSequential&& other);
        VertexPainterSequential& operator=(const VertexPainterSequential& other);
        VertexPainterSequential& operator=(VertexPainterSequential&& other);
        
        // Getters
        float get_factor() const;

        // Setter
        void set_factor(float factor);
        
        // Paint 'vertices' according to the order of the vertices in the
        // 'vertices' vector.
        // 'bounding_box', 'iteration_of_vertices' and 'max_recursion' are not used.
        virtual void paint_vertices(std::vector<sf::Vertex>& vertices,
                                    const std::vector<int>& iteration_of_vertices,
                                    int max_recursion,
                                    sf::FloatRect bounding_box) const override;


    private:
        // Implements the deep-copy cloning.
        virtual std::shared_ptr<VertexPainter> clone_impl() const override;

        float factor_ {0};
    };
}


#endif // VERTEX_PAINTER_SEQUENTIAL_H
