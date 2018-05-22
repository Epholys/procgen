#ifndef VERTEX_PAINTER_H
#define VERTEX_PAINTER_H


#include <memory>
#include <SFML/Graphics.hpp>
#include "ColorsGenerator.h"

namespace colors
{
    class VertexPainter
    {
    public:
        VertexPainter();
        VertexPainter(std::shared_ptr<ColorGenerator> gen);
        
        void paint_vertices(std::vector<sf::Vertex>& vertices, sf::FloatRect bounding_box);
        
    private:
        void swap(VertexPainter& other);

        std::shared_ptr<ColorGenerator> generator_;
    };
}


#endif // VERTEX_PAINTER_H


