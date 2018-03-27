#ifndef GEOMETRY_H
#define GEOMETRY_H


#include <vector>
#include <SFML/Graphics.hpp>

namespace geometry
{
    // Compute the bounding box of a set of vertices.
    // Complexity in time is in O(n), n being the number of vertices.
    sf::FloatRect compute_bounding_box(const std::vector<sf::Vertex>& vertices);

    // Divide the vertices into 'max_boxes_'-1 equal part (with a remainder) and
    // compute the bounding boxes of each part. It is used to have a more
    // fitting "hitbox" of a set of vertices. The hitboxes overlap by one
    // vertex to not have any edge left out.
    // 
    // Complexity in time is in O(n), n being the number of vertices.
    //
    // Note: The algorithm breaks for low count of vertices: it returns a
    // correct set of bounding boxes but 'max_boxes' is not respected. See the
    // code for more informations.
    std::vector<sf::FloatRect> compute_sub_boxes(const std::vector<sf::Vertex>& vertices,
                                                 int max_boxes);
}


#endif //  GEOMETRY_H
