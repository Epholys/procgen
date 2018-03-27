#ifndef GEOMETRY_H
#define GEOMETRY_H


#include <vector>
#include <SFML/Graphics.hpp>

namespace geometry
{
    sf::FloatRect compute_bounding_box(const std::vector<sf::Vertex>& vertices);
    std::vector<sf::FloatRect> compute_sub_boxes(const std::vector<sf::Vertex>& vertices,
                                                 int max_boxes);
}


#endif //  GEOMETRY_H
