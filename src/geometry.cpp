#include <gsl/gsl>
#include "geometry.h"
#include "helper_math.h"

namespace geometry
{
    sf::FloatRect compute_bounding_box(const std::vector<sf::Vertex>& vertices)
    {
        if (vertices.size() == 0)
        {
            return { 0, 0, 0, 0 };
        }
        const auto& first = vertices.at(0);
        // Warning: 'top' is at low value because of the axes defined by SFML.
        float top = first.position.y, down = first.position.y;
        float left = first.position.x, right = first.position.x;

        // For each vertices, update the bounding box coordinates if necessary.
        for (const auto& v : vertices)
        {
            if (v.position.y < top)
            {
                top = v.position.y;
            }
            else if (v.position.y > down)
            {
                down = v.position.y;
            }

            if (v.position.x > right)
            {
                right = v.position.x;
            }
            else if (v.position.x < left)
            {
                left = v.position.x;
            }
        }
        return {left, top, right - left, down - top};
    }
    
    std::vector<sf::FloatRect> compute_sub_boxes(const std::vector<sf::Vertex>& vertices,
                                                 int max_boxes)
    {
        Expects(max_boxes > 0);

        // A 'max_boxes' of 1 and 2 are equivalent and correspond to a single
        // bounding_box. 
        if (max_boxes == 1)
        {
            max_boxes = 2;
        }

        std::vector<sf::FloatRect> boxes;

        // Each bounding_box must have rougly the same number of
        // vertices. However, it can not be exact: the number of vertices may
        // not a multiple of the number of boxes. As a consequence the last
        // boxes will be the remainder of the division. That's why, to respect
        // the number of 'max_boxes', we divide by 'max_boxes-1'.
        // Edge case: If 'max_boxes' is equal to 1 or 2, it will be a single
        // bounding box, as there are not any remainder.
        int vertices_per_box = vertices.size()  / (max_boxes-1);

        // The algorithm makes overlapping boxes. We must have a least 3
        // vertices per box.
        // As it overlaps, several vertices are shared, so the effective number
        // of vertices is greater than the real number. As a consequence, for
        // low count of vertices, it returns a number of boxes greater than
        // 'max_boxes'
        vertices_per_box = vertices_per_box < 3 ? 3 : vertices_per_box;
        
        int n = 0;
        std::vector<sf::Vertex> box_vertices;
        for (size_t i = 0; i<vertices.size(); ++i)
        {
            // Create a box when the number of vertices is attained
            if (n == vertices_per_box)
            {
                n = 0;
                i -= 2; // Go back to count several time the number of vertices
                        // to make overlapping boxes
                boxes.push_back(compute_bounding_box(box_vertices));
                box_vertices.clear();
            }

            // Add a vertex to the next box.
            box_vertices.push_back(vertices.at(i));
            ++n;

            // For the final box, the remainder of the vertices does not attain
            // 'vertices_per_box', so manually set it.
            if (i == vertices.size()-1)
            {
                boxes.push_back(compute_bounding_box(box_vertices));
            }
        }

        return boxes;
    }

    sf::Vector2f intersection(const Line& l1, const Line& l2)
    {
        sf::Vector2f a = l1.point, b = l2.point;
        sf::Vector2f u = l1.direction, v = l2.direction;
        sf::Vector2f intersection = a;
        
        float dx = b.x - a.x;
        float dy = b.y - a.y;
        float det = v.x * u.y - v.y * u.x;
        if (det != 0)
        {
            float t = (dy * v.x - dx * v.y) / det;
            intersection = a + t * u;
        }
        return intersection;
    }

    float distance (const sf::Vector2f& a, const sf::Vector2f& b)
    {
        return std::sqrt(std::pow(b.x-a.x, 2)+std::pow(b.y-a.y, 2));
    }

    float angle_from_vector(const sf::Vector2f vec)
    {
         // y axis is downward
        if (vec.x == 0 && vec.y <= 0)
        {
            return math::pi / 2;
        }
        else if (vec.x == 0 && vec.y > 0)
        {
            return -math::pi / 2;
        }
        else
        {
            return std::atan(-vec.y / vec.x);
        }
    }
    
    std::pair<sf::Vector2f, sf::Vector2f> intersection_with_bounding_box(const Line& line,
                                                                         const sf::FloatRect& bounding_box)
    {
        // From the bounding box, define the four lines that make up the box. 
        enum bound { Upper=0, Rightmost, Bottom, Leftmost };
        std::vector<Line> bounds (4);
        bounds.at(Upper) = {{bounding_box.left, bounding_box.top}, {1,0}};
        bounds.at(Rightmost) = {{bounding_box.left+bounding_box.width, bounding_box.top}, {0,1}};
        bounds.at(Bottom) = {{bounding_box.left, bounding_box.top+bounding_box.height}, {1,0}};
        bounds.at(Leftmost) = {{bounding_box.left, bounding_box.top}, {0,1}};

        float angle = angle_from_vector(line.direction);

        std::pair<Line, Line> possible_intersection_line; // at the direction pointed by the angle
        std::pair<Line, Line> possible_opposite_intersection_line; // at the direction pointed by the angle
        if (angle >= 0. && angle < 180.)
        {
            possible_intersection_line.first = bounds.at(Upper);
            possible_opposite_intersection_line.first = bounds.at(Bottom);
        }
        else
        {
            possible_intersection_line.first = bounds.at(Bottom);
            possible_opposite_intersection_line.first = bounds.at(Upper);
        }
        if (angle >= 90. && angle < 270.)
        {
            possible_intersection_line.second = bounds.at(Leftmost);
            possible_opposite_intersection_line.second = bounds.at(Rightmost);
        }
        else
        {
            possible_intersection_line.second = bounds.at(Rightmost);
            possible_opposite_intersection_line.second = bounds.at(Leftmost);
        }

        std::pair<sf::Vector2f, sf::Vector2f> possible_intersection = {line.point, line.point};
        possible_intersection.first = intersection(line, possible_intersection_line.first);
        possible_intersection.second = intersection(line, possible_intersection_line.second);

        std::pair<sf::Vector2f, sf::Vector2f> possible_opposite_intersection = {line.point, line.point};
        possible_opposite_intersection.first = intersection(line, possible_opposite_intersection_line.first);
        possible_opposite_intersection.second = intersection(line, possible_opposite_intersection_line.second);

        sf::Vector2f intersection {line.point};
        if (distance(line.point, possible_intersection.first) <= distance(line.point, possible_intersection.second))
        {
            intersection = possible_intersection.first;
        }
        else
        {
            intersection = possible_intersection.second;
        }

        sf::Vector2f opposite_intersection {line.point};
        if (distance(line.point, possible_opposite_intersection.first) <= distance(line.point, possible_opposite_intersection.second))
        {
            opposite_intersection = possible_opposite_intersection.first;
        }
        else
        {
            opposite_intersection = possible_opposite_intersection.second;
        }

        return {intersection, opposite_intersection};
    }

    sf::Vector2f projection(sf::Vector2f A, sf::Vector2f B, sf::Vector2f P)
    {
        sf::Vector2f AB = B - A;
        float AB_squared = AB.x*AB.x + AB.y*AB.y;
        if (AB_squared == 0)
        {
            return A;
        }

        sf::Vector2f AP = P - A;
        float t = (AP.x*AB.x + AP.y*AB.y) / AB_squared;
        sf::Vector2f projection;
        if (t < 0.)
        {
            projection = A;
        }
        else if (t > 1.)
        {
            projection = B;
        }
        else
        {
            projection = A + t*AB;
        }
        return projection;
    }

}
