#ifndef VERTEX_PAINTER_RECURSION_H
#define VERTEX_PAINTER_RECURSION_H


#include "VertexPainter.h"

namespace colors
{
    class VertexPainterRecursion : public VertexPainter
    {
    public:
        VertexPainterRecursion(); // Create a default generator
        explicit VertexPainterRecursion(const std::shared_ptr<ColorGenerator> gen);
        // Shallow rule-of-five constructors.
        VertexPainterRecursion(const VertexPainterRecursion& other);
        VertexPainterRecursion(VertexPainterRecursion&& other);
        VertexPainterRecursion& operator=(const VertexPainterRecursion& other);
        VertexPainterRecursion& operator=(VertexPainterRecursion&& other);
        
        // Paint 'vertices' according to its recursion value: simlpy divide the
        // current recursion by the max recursion.
        virtual void paint_vertices(std::vector<sf::Vertex>& vertices,
                                    const std::vector<int>& vertices_recursion,
                                    int max_recursion,
                                    sf::FloatRect bounding_box) const override;

    private:
        // Implements the deep-copy cloning.
        virtual std::shared_ptr<VertexPainter> clone_impl() const override;
    };
}


#endif // VERTEX_PAINTER_RECURSION_H
