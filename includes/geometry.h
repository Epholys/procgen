#ifndef GEOMETRY_H
#define GEOMETRY_H


#include <cmath>
#include <vector>
#include <SFML/Graphics.hpp>


// This namespace contains all utilities related to geometric operations.
// WARNING: all of this was created ad-hoc and as a consequence is not coherent.
// TODO: if this file continues expending, do a big makeover to have a cohesive
// system.
namespace geometry
{
    // Define a Line as a point and a direction vector.
    struct Line
    {
        sf::Vector2f point;
        sf::Vector2f direction;
    };

    // Computes the euclidean distance between the points 'a' and 'b'.
    float distance(const sf::Vector2f& a, const sf::Vector2f& b);

    // Computes the intersection between the lines 'l1' and 'l2'.
    sf::Vector2f intersection(const Line& l1, const Line& l2);

    // Computes the angle between the x axis and the vector 'vec' (in radian).
    float angle_from_vector(const sf::Vector2f vec);

    // TODO
    sf::Vector2f project(sf::Vector2f A, sf::Vector2f B, sf::Vector2f p);
    // First projects the point A on the straight line (AB) then clamp it to A
    // or B if the projected point is not in the segment [AB].
    sf::Vector2f project_and_clamp(sf::Vector2f A, sf::Vector2f B, sf::Vector2f p);

    // Compute the bounding box of a set of vertices.
    // Complexity in time is in O(n), n being the number of vertices.
    sf::FloatRect bounding_box(const std::vector<sf::Vertex>& vertices);

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
    std::vector<sf::FloatRect> sub_boxes(const std::vector<sf::Vertex>& vertices,
                                         int max_boxes);

    // Expand 'boxes' by 'expension' in all directions.
    void expand_boxes(std::vector<sf::FloatRect>& boxes, float expension=5.f);

    // Computes the two intersections 'line' forms with 'bounding_box'. The
    // first component of the pair is the intersection closest to the 'point'
    // attribute of 'line'. The second component is farthest one.
    //
    // Exception:
    //   - Precondition: the 'point' attribute of 'line' must be inside the 'bounding_box'.
    std::pair<sf::Vector2f, sf::Vector2f> intersection_with_bounding_box(const Line& line,
                                                                         const sf::FloatRect& bounding_box);
}


#endif //  GEOMETRY_H
