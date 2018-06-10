#include <gsl/gsl>
#include "geometry.h"

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

    bool intersection (const sf::Vector2f& a, const sf::Vector2f& u, const sf::Vector2f& b, const sf::Vector2f v, sf::Vector2f& intersection)
    {
        float dx = b.x - a.x;
        float dy = b.y - a.y;
        float det = v.x * u.y - v.y * u.x;
        if (det != 0)
        {
            float t = (dy * v.x - dx * v.y) / det;
            intersection = a + t * u;
            return true;
        }
        return false;
    }

    float distance (const sf::Vector2f& a, const sf::Vector2f& b)
    {
        return std::sqrt(std::pow(b.x-a.x, 2)+std::pow(b.y-a.y, 2));
    }
}
