#ifndef VERTEX_PAINTER_H
#define VERTEX_PAINTER_H


#include "ColorsGeneratorWrapper.h"
#include "cereal/cereal.hpp"
#include "cereal/types/polymorphic.hpp"
#include "types.h"

#include <SFML/Graphics.hpp>
#include <memory>


namespace colors
{
// Paint the vertices according to a rule with a ColorGenerator.
// For example, paints according to radial gradient with a ColorGenerator of
// green hues.
// VertexPainter is a base class with pure virtual methods, the child
// classes implement the coloring rules. It observes a ColorGeneratorWrapper
// and pass the 'notify()' call to the Observers.
class VertexPainter : public Indicator
{
  public:
    VertexPainter() = default; // Create a default generator
    virtual ~VertexPainter() {};
    explicit VertexPainter(ColorGeneratorWrapper wrapper);
    // Delete the copy/move constructors to avoid slicing
    VertexPainter(const VertexPainter&) = delete;
    VertexPainter(VertexPainter&&) = delete;
    VertexPainter& operator=(const VertexPainter&) = delete;
    VertexPainter& operator=(VertexPainter&&) = delete;

    // Clone method to do a deep copy of the 'this' object with the correct
    // child wrapped in this polymorphic pointer.
    virtual std::shared_ptr<VertexPainter> clone() const = 0;

    // Getters/Setters
    const ColorGeneratorWrapper& get_generator_wrapper() const;
    ColorGeneratorWrapper& ref_generator_wrapper();
    void set_generator_wrapper(const ColorGeneratorWrapper& color_generator_wrapper);

    // Method to be called every frame to draw some helper visuals.
    // 'bounding_box' is the only necessary information for now, but all
    // other parameters of 'paint_vertices()' could be used in the future.
    virtual void supplementary_drawing(sf::FloatRect bounding_box) const;

    // Paint 'vertices' with the informations of all the other parameters
    // according to a rule with the colors from
    // 'ColorGeneratorWrapper::ColorGenerator'.
    virtual void paint_vertices(std::vector<sf::Vertex>& vertices,
                                const std::vector<u8>& iteration_of_vertices,
                                const std::vector<bool>& transparent,
                                int max_recursion,
                                sf::FloatRect bounding_box) = 0;

    virtual std::string type_name() const = 0;

    virtual bool poll_modification() override;

  protected:
    ColorGeneratorWrapper generator_ {};
};
} // namespace colors

#endif // VERTEX_PAINTER_H
