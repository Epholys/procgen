#ifndef HELPER_MATH_H
#define HELPER_MATH_H

#include <cmath>
#include<SFML/System.hpp>

// This namespace defines commonly used function and constants missing in <cmath>.
namespace math
{
    const double pi = std::acos(-1.0L);

    double degree_to_rad (double deg);
    double rad_to_degree (double rad);

    float degree_to_rad (float deg);
    float rad_to_degree (float rad);
}

namespace ext::sf
{
    using Vector2d = ::sf::Vector2<double>;
}

#endif
