#ifndef GEOMETRY_H
#define GEOMETRY_H


#include <cmath>
#include <vector>
#include <SFML/Graphics.hpp>

namespace geometry
{
    // Define a Line as a point and a direction vector.
    struct Line
    {
        sf::Vector2f point;
        sf::Vector2f direction;
    };

    // TODO: remove 'compute'
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
    // implementation code for more informations.
    std::vector<sf::FloatRect> compute_sub_boxes(const std::vector<sf::Vertex>& vertices,
                                                 int max_boxes);


    // Compute the intersection between the 
    sf::Vector2f intersection(const Line& l1, const Line& l2);
    
    float distance(const sf::Vector2f& a, const sf::Vector2f& b);
    float angle_from_vector(const sf::Vector2f vec);

    std::pair<sf::Vector2f, sf::Vector2f> intersection_with_bounding_box(const Line& line,
                                                                         const sf::FloatRect& bounding_box);
    sf::Vector2f projection(sf::Vector2f A, sf::Vector2f B, sf::Vector2f p);
}


#endif //  GEOMETRY_H
