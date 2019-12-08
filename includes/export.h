#ifndef EXPORT_H
#define EXPORT_H


#include <vector>
#include <string>

namespace sf
{
    class Vertex;
}

namespace drawing
{
    std::vector<sf::Vertex> add_width(const std::vector<sf::Vertex>& v, float w);
    bool export_to_png (const std::string& filename);
}


#endif // EXPORT_H
