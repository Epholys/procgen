#ifndef VERTEX_PAINTER_LINEAR_H
#define VERTEX_PAINTER_LINEAR_H


#include "VertexPainter.h"

namespace colors
{
    // Paint the vertices according to a rule with a ColorGenerator.
    // For example, paints according to radial gradient with a ColorGenerator of
    // green hues.
    class VertexPainterLinear : public VertexPainter
    {
    public:
        VertexPainterLinear(); // Create a default generator
        explicit VertexPainterLinear(const std::shared_ptr<ColorGenerator> gen);
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
        // from 'generator_'.
        virtual void paint_vertices(std::vector<sf::Vertex>& vertices, sf::FloatRect bounding_box) const override;

    private:
        virtual std::shared_ptr<VertexPainter> clone_impl() const override;

        float angle_ {0};
    };
}


#endif // VERTEX_PAINTER_LINEAR_H
