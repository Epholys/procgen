#ifndef VERTEX_PAINTER_H
#define VERTEX_PAINTER_H


#include <memory>
#include <SFML/Graphics.hpp>
#include "Observable.h"
#include "ColorsGenerator.h"

namespace colors
{
    class VertexPainter : public Observable
    {
    public:
        VertexPainter();
        VertexPainter(std::shared_ptr<ColorGenerator> gen);
        
        void paint_vertices(std::vector<sf::Vertex>& vertices, sf::FloatRect bounding_box);

        void interact_with();

    private:
        std::shared_ptr<ColorGenerator> generator_;
    };
}


#endif // VERTEX_PAINTER_H


