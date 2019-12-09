#ifndef EXPORT_H
#define EXPORT_H


#include <vector>
#include <string>

namespace sf
{
    class Vertex;
}

namespace procgui
{
    class LSystemView;
}

namespace drawing
{
    std::vector<sf::Vertex> add_width(const std::vector<sf::Vertex>& v, float w);
    bool export_to_png (procgui::LSystemView view,
                        const std::string& filename,
                        int n_iter,
                        int image_dim,
                        double ratio);
}


#endif // EXPORT_H
