#include "VertexPainter.h"

namespace colors
{
    VertexPainter::VertexPainter()
        : generator_{nullptr}
    {
        generator_ = std::make_shared<LinearGradient>(LinearGradient::keys({{sf::Color::Red, 0.}, {sf::Color::Green, 0.75}, {sf::Color::Blue, 1.}}));
    }
    VertexPainter::VertexPainter(std::shared_ptr<ColorGenerator> gen)
        : generator_{gen}
    {
    }
    
    void VertexPainter::paint_vertices(std::vector<sf::Vertex>& vertices, sf::FloatRect bounding_box, sf::Transform transform)
    {
        if (!generator_)
        {
            return;
        }

        // If necessary, for now yes
        std::vector<sf::Vertex> transformed_vertices;
        for (const auto& v : vertices)
        {
            sf::Vertex tv = v;
            tv.position = transform.transformPoint(tv.position);
            transformed_vertices.push_back(tv);
        }
        bounding_box = transform.transformRect(bounding_box);
        
        float top = bounding_box.top;
        float bottom = top + bounding_box.height;

        for (size_t i=0; i<vertices.size(); ++i)
        {
            auto color = generator_->get((bottom - transformed_vertices.at(i).position.y) / (bottom - top));
            color.a = vertices.at(i).color.a;
            vertices.at(i).color = color;
        }
    }
}
