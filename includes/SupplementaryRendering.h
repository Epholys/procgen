#ifndef SUPPLEMENTARY_RENDERING_H
#define SUPPLEMENTARY_RENDERING_H


#include <SFML/Graphics.hpp>

namespace procgui
{
// Globally accessible static singleton class to add draw calls everywhere
// in the code.
//
// This is a static singleton because it can't logically be a member of
// another class, and it will be way too cumbersome to add it as a parameter
// everywhere. The global accessibility side-effects nightmare is not really
// catastrophic in this case, as only adding a draw call and clearing all is
// possible.
class SupplementaryRendering
{
  public:
    struct DrawCall
    {
        std::vector<sf::Vertex> vertices {};
        sf::PrimitiveType type {sf::PrimitiveType::Lines};
        sf::RenderStates states {sf::RenderStates::Default};
    };

    // Delete the constructor to have static singleton status.
    SupplementaryRendering() = delete;

    // Add a 'call' to the global vector.
    static void add_draw_call(const DrawCall& call);

    // Clear the draw call vector
    static void clear_draw_calls();

    // Draw all the draw calls to 'target'
    static void draw(sf::RenderTarget& target);

  private:
    static std::vector<DrawCall> draw_calls_;
};
} // namespace procgui


#endif // SUPPLEMENTARY_RENDERING_H
