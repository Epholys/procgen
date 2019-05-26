#include <cmath>
#include <gtest/gtest.h>
#include "helper_math.h"
#include "geometry.h"

using namespace geometry;

// Create a circle
std::vector<sf::Vertex> gen_circle()
{
    const float PI = 3.1415;
    std::vector<sf::Vertex> circle;
    for(float i = 0; i <= 2*PI; i += PI/12.f)
    {
        circle.push_back({{std::cos(i), std::sin(i)}});
    }
    return circle;
}

TEST(geometry, distance)
{
    sf::Vector2f a {0,0};
    sf::Vector2f b {1,1};

    ASSERT_FLOAT_EQ(std::sqrt(2), distance(a, b));
}

TEST(geometry, intersection)
{
    Line l1 {{0,1}, {1,-1}};
    Line l2 {{2,1}, {-1,-1}};

    auto intersect = geometry::intersection(l1, l2);
    ASSERT_FLOAT_EQ(1, intersect.x);
    ASSERT_FLOAT_EQ(0, intersect.y);
}

TEST(geometry, angle_from_vector)
{
    ASSERT_FLOAT_EQ(math::pi / 2, angle_from_vector(sf::Vector2f(0,-1))); 
    ASSERT_FLOAT_EQ(3 * math::pi / 2, angle_from_vector(sf::Vector2f(0,1))); 
    ASSERT_NEAR(math::degree_to_rad(45.f), angle_from_vector(sf::Vector2f(1,1)), 0.0001); 
}

TEST(geometry, project)
{
    sf::Vector2f A (0, 0);
    sf::Vector2f B (2, 2);
    sf::Vector2f P (0, 2);

    auto proj = project_and_clamp(A, B, P);
    ASSERT_FLOAT_EQ(proj.x, 1);
    ASSERT_FLOAT_EQ(proj.y, 1);
}

TEST(geometry, project_and_clamp)
{
    sf::Vector2f A (0, 0);
    sf::Vector2f B (2, 2);
    sf::Vector2f P (0, 5);

    auto proj = project_and_clamp(A, B, P);
    ASSERT_FLOAT_EQ(proj.x, 2);
    ASSERT_FLOAT_EQ(proj.y, 2);
}

TEST(geometry, bounding_box)
{
    auto circle = gen_circle();
    auto box = bounding_box(circle);
    ASSERT_EQ(sf::FloatRect(-1, -1, 2, 2), box);
}

// sub_boxes is not tested due to the bad behaviour with a low count of
// vertices. 

TEST(geometry, expand)
{
    std::vector<sf::FloatRect> boxes { {0, 0, 1, 1}, {10, 10, 10, 10 } };
    std::vector<sf::FloatRect> expected_boxes { {-10, -10, 21, 21}, {0, 0, 30, 30} };

    expand_boxes(boxes, 10.);

    ASSERT_EQ(expected_boxes, boxes);
}

TEST(geometry, intersection_with_bounding_box)
{
    Line l {{2, 1}, {1, -1}};
    sf::FloatRect rect {0, 0, 4, 4};
    auto intersections = intersection_with_bounding_box(l, rect);
    ASSERT_FLOAT_EQ(intersections.first.x, 3);
    ASSERT_FLOAT_EQ(intersections.first.y, 0);
    ASSERT_FLOAT_EQ(intersections.second.x, 0);
    ASSERT_FLOAT_EQ(intersections.second.y, 3);
}
