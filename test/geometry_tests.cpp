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

TEST(geometry, bounding_box)
{
    auto circle = gen_circle();
    auto box = compute_bounding_box(circle);
    ASSERT_EQ(sf::FloatRect(-1, -1, 2, 2), box);
}

// compute_sub_boxes is not tested due to the bad behaviour with a low count of
// vertices. 
