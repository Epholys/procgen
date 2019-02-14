#ifndef VERTEX_PAINTER_H
#define VERTEX_PAINTER_H


#include <memory>
#include <SFML/Graphics.hpp>
#include "cereal/cereal.hpp"
#include "cereal/types/polymorphic.hpp"
#include "Observable.h"
#include "Observer.h"
#include "ColorsGeneratorWrapper.h"


namespace colors
{
    // Paint the vertices according to a rule with a ColorGenerator.
    // For example, paints according to radial gradient with a ColorGenerator of
    // green hues.
    // VertexPainter is a base class with pure virtual methods, the child
    // classes implement the coloring rules. It observes a ColorGeneratorWrapper
    // and pass the 'notify()' call to the Observers.
    class VertexPainter : public Observable
                        , public Observer<ColorGeneratorWrapper>
    {
    public:
        using OGenBuff = Observer<ColorGeneratorWrapper>;

        VertexPainter(); // Create a default generator
        virtual ~VertexPainter() {};
        explicit VertexPainter(const std::shared_ptr<ColorGenerator> gen);
        // Rule-of-five shallow copy
        VertexPainter(const VertexPainter& other);
        VertexPainter(VertexPainter&& other);
        VertexPainter& operator=(const VertexPainter& other);
        VertexPainter& operator=(VertexPainter&& other);

        // Clone method to do a deep copy of the 'this' object with the correct
        // child class. Calls 'clone_impl()' internally, so the child class can
        // copy itself into a polymorphic pointer.
        std::shared_ptr<VertexPainter> clone() const;
        
        // Getters/Setters
        std::shared_ptr<ColorGeneratorWrapper> get_generator_wrapper() const;
        void set_generator_wrapper(std::shared_ptr<ColorGeneratorWrapper> color_generator_wrapper);

        // Paint 'vertices' with the informations of 'bounding_box' and
        // 'iteration_of_vertices' according to a rule with the colors from
        // 'ColorGeneratorWrapper::ColorGenerator'.
        virtual void paint_vertices(std::vector<sf::Vertex>& vertices,
                                    const std::vector<int>& iteration_of_vertices,
                                    int max_recursion,
                                    sf::FloatRect bounding_box) = 0;

    private:
        // Clone implementation.
        virtual std::shared_ptr<VertexPainter> clone_impl() const = 0;
    };
}

#endif // VERTEX_PAINTER_H
