#ifndef VERTEX_PAINTER_ITERATION_H
#define VERTEX_PAINTER_ITERATION_H


#include "VertexPainter.h"

namespace colors
{
    class VertexPainterIteration : public VertexPainter
    {
    public:
        VertexPainterIteration(); // Create a default generator
        explicit VertexPainterIteration(const std::shared_ptr<ColorGenerator> gen);
        // Shallow rule-of-five constructors.
        VertexPainterIteration(const VertexPainterIteration& other);
        VertexPainterIteration(VertexPainterIteration&& other);
        VertexPainterIteration& operator=(const VertexPainterIteration& other);
        VertexPainterIteration& operator=(VertexPainterIteration&& other);
        
        // Paint 'vertices' according to its iteration value: simply divide the
        // current iteration by the max iteration.
        // 'bounding_box' is not used.
        virtual void paint_vertices(std::vector<sf::Vertex>& vertices,
                                    const std::vector<int>& vertices_iteration,
                                    int max_iteration,
                                    sf::FloatRect bounding_box) override;

    private:
        // Implements the deep-copy cloning.
        virtual std::shared_ptr<VertexPainter> clone_impl() const override;
    };
}


#endif // VERTEX_PAINTER_ITERATION_H
