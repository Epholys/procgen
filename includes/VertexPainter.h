#ifndef VERTEX_PAINTER_H
#define VERTEX_PAINTER_H


#include <memory>
#include <SFML/Graphics.hpp>
#include "Observable.h"
#include "Observer.h"
#include "ColorsGenerator.h"

namespace colors
{
    // TODO: Stub class, rewrites comments.
    // Paint the vertices according to a rule with a ColorGenerator.
    // For example, paints according to radial gradient with a ColorGenerator of
    // green hues.
    // No invariants.
    class VertexPainter : public Observable
                        , public Observer<ColorGenerator>
    {
    public:

        VertexPainter(); // Create a default generator
        VertexPainter(const std::shared_ptr<ColorGenerator> gen); // Copy the generator
        VertexPainter(const VertexPainter& other);
        VertexPainter(VertexPainter&& other);
        VertexPainter& operator=(VertexPainter other);
        
        float get_angle() const;
        void set_angle(float angle);
        const std::shared_ptr<ColorGenerator> get_generator() const;
        std::shared_ptr<ColorGenerator>& ref_generator();
        void set_generator(std::shared_ptr<ColorGenerator> generator);
        
        // Paint 'vertices' with the informations of 'bounding_box' according to
        // the rule with the colors from generator_
        void paint_vertices(std::vector<sf::Vertex>& vertices, sf::FloatRect bounding_box) const;

    private:
        void swap(VertexPainter& other);
        
        std::shared_ptr<ColorGenerator> generator_;
        float angle_ {60.};
    };
}


#endif // VERTEX_PAINTER_H


