#include "VertexPainter.h"
#include "procgui.h"

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
    
    void VertexPainter::paint_vertices(std::vector<sf::Vertex>& vertices, sf::FloatRect bounding_box)
    {
        if (!generator_)
        {
            return;
        }
        
        float top = bounding_box.top;
        float bottom = top + bounding_box.height;
        for (auto& v : vertices)
        {
            auto color = generator_->get((bottom - v.position.y) / (bottom - top));
            color.a = v.color.a;
            v.color = color;
        }
    }

    bool VertexPainter::interact_with()
    {
        return procgui::interact_with(generator_, "");
    }
}
