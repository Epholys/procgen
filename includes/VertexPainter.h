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
    // TODO: Class will be modified, rewrites comments.
    // Paint the vertices according to a rule with a ColorGenerator.
    // For example, paints according to radial gradient with a ColorGenerator of
    // green hues.
    // Invariant: the ColorGenerator pointer of Observer must point to the same
    // ColorGenerator than the ColorGeneratorBuffer. However, it can not be
    // implemented in this class, so ColorGeneratorBuffer takes care of it.
    class VertexPainter : public Observable
                         // The generator itself, forward the notify() to
                         // LSystemView to paint again.
                        , public Observer<ColorGenerator>
                         // The GeneratorBuffer, to update the
                         // Observer<ColorGenerator> when a new one is
                         // constructed.
                        , public Observer<ColorGeneratorBuffer>
    {
    public:
        using OGen = Observer<ColorGenerator>;
        using OBuff = Observer<ColorGeneratorBuffer>;

        VertexPainter(); // Create a default generator
        // TODO Rule-of-five deep copy.
        VertexPainter(const std::shared_ptr<ColorGenerator> gen);
        VertexPainter(const VertexPainter& other);
        VertexPainter(VertexPainter&& other);
        VertexPainter& operator=(VertexPainter other);
        
        // Getters
        float get_angle() const;
        std::shared_ptr<ColorGeneratorBuffer> get_generator_buffer() const;

        // Setter
        void set_angle(float angle);

        // Reconcile the pointers of OGen and OBuff::ColorGenerator.
        void update_generator();
        
        // Paint 'vertices' with the informations of 'bounding_box' according to
        // the rule with the colors from 'generator_'
        void paint_vertices(std::vector<sf::Vertex>& vertices, sf::FloatRect bounding_box) const;

    private:
        void swap(VertexPainter& other);
        
        const std::shared_ptr<ColorGeneratorBuffer>& generator_buffer_;
        float angle_ {0};
    };
}


#endif // VERTEX_PAINTER_H


