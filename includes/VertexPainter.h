#ifndef VERTEX_PAINTER_H
#define VERTEX_PAINTER_H


#include <memory>
#include <SFML/Graphics.hpp>
#include "Observable.h"
#include "Observer.h"
#include "ColorsGenerator.h"
#include "ColorsGeneratorBuffer.h"

namespace colors
{
    // Paint the vertices according to a rule with a ColorGenerator.
    // For example, paints according to radial gradient with a ColorGenerator of
    // green hues.
    class VertexPainter : public Observable
                        , public Observer<ColorGeneratorBuffer>
    {
    public:
        using OGenBuff = Observer<ColorGeneratorBuffer>;

        VertexPainter(); // Create a default generator
        explicit VertexPainter(const std::shared_ptr<ColorGenerator> gen);
        // Rule-of-five shallow copy
        VertexPainter(const VertexPainter& other);
        VertexPainter(VertexPainter&& other);
        VertexPainter& operator=(const VertexPainter& other);
        VertexPainter& operator=(VertexPainter&& other);

        std::shared_ptr<VertexPainter> clone() const;
        
        // Getters
        std::shared_ptr<ColorGeneratorBuffer> get_generator_buffer() const;

        // Paint 'vertices' with the informations of 'bounding_box' according to
        // the rule with the colors from 'generator_'
        virtual void paint_vertices(std::vector<sf::Vertex>& vertices, sf::FloatRect bounding_box) const = 0;

    private:
        virtual std::shared_ptr<VertexPainter> clone_impl() const = 0;
        
    };
}


#endif // VERTEX_PAINTER_H


