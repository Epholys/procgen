#ifndef VERTEX_PAINTER_RADIAL_H
#define VERTEX_PAINTER_RADIAL_H


#include "VertexPainter.h"

namespace colors
{
    class VertexPainterRadial : public VertexPainter
    {
    public:
        VertexPainterRadial(); // Create a default generator
        explicit VertexPainterRadial(const std::shared_ptr<ColorGenerator> gen);
        // Shallow rule-of-five constructors.
        VertexPainterRadial(const VertexPainterRadial& other);
        VertexPainterRadial(VertexPainterRadial&& other);
        VertexPainterRadial& operator=(const VertexPainterRadial& other);
        VertexPainterRadial& operator=(VertexPainterRadial&& other);
        
        // Getters
        sf::Vector2f get_center() const;

        // Setter
        void set_center(sf::Vector2f center);
        
        // Paint 'vertices' in a 'center_' centered distance-bases radial
        // fashion with the informations of 'bounding_box' according to the rule
        // with the colors from the ColorGenerator.
        virtual void paint_vertices(std::vector<sf::Vertex>& vertices, sf::FloatRect bounding_box) const override;

    private:
        // Implements the deep-copy cloning.
        virtual std::shared_ptr<VertexPainter> clone_impl() const override;

        // [0, 1] center: 0 is the left/up of the bounding_box and 1 the right/down.
        sf::Vector2f center_ {0, 0};
    };
}

#endif // VERTEX_PAINTER_RADIAL_H
