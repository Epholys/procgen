#ifndef SUPPLEMENTARY_RENDERING_H
#define SUPPLEMENTARY_RENDERING_H


#include <SFML/Graphics.hpp>

namespace procgu
{
    class SupplementaryRendering
    {
    public:
        struct DrawCall
        {
            std::vector<sf::Vertex> vertices {};
            sf::PrimitiveType type {sf::PrimitiveType::Lines};
            sf::RenderStates states {sf::RenderStates::Default};
        };

        SupplementaryRendering() = delete;

        static void add_draw_call(const DrawCall& call);
        static void clear_draw_calls();

    private:
        static std::vector<DrawCall> draw_calls_;
    };
}


#endif // SUPPLEMENTARY_RENDERING_H
