#ifndef VERTEX_PAINTER_LINEAR_H
#define VERTEX_PAINTER_LINEAR_H


#include "VertexPainter.h"

namespace colors
{
    // Paint the vertices with a ColorGenerator following a line passing through
    // the center of the bounding box at a certain 'angle_'.
    class VertexPainterLinear : public VertexPainter
    {
    public:
        VertexPainterLinear(); // Create a default generator
        explicit VertexPainterLinear(const std::shared_ptr<ColorGenerator> gen);
        // Shallow rule-of-five constructors.
        VertexPainterLinear(const VertexPainterLinear& other);
        VertexPainterLinear(VertexPainterLinear&& other);
        VertexPainterLinear& operator=(const VertexPainterLinear& other);
        VertexPainterLinear& operator=(VertexPainterLinear&& other);
        
        // Getters
        float get_angle() const;

        // Setter
        void set_angle(float angle);
        
        // Paint 'vertices' in a center-oriented linear gradient with the
        // informations of 'bounding_box' according to the rule with the colors
        // from the ColorGenerator.
        virtual void paint_vertices(std::vector<sf::Vertex>& vertices, sf::FloatRect bounding_box) const override;

    private:
        // Implements the deep-copy cloning.
        virtual std::shared_ptr<VertexPainter> clone_impl() const override;

        float angle_ {0};
    };
}


#endif // VERTEX_PAINTER_LINEAR_H
