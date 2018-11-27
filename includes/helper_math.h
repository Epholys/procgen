#ifndef HELPER_MATH_H
#define HELPER_MATH_H
#include<random>

#include <cmath>
#include<SFML/System.hpp>

// This namespace defines commonly used function and constants missing in <cmath>.
namespace math
{
    static const double pi = std::acos(-1.0L);
    static std::random_device random_dev;
    static std::mt19937 random_generator(random_dev());

    double degree_to_rad (double deg);
    double rad_to_degree (double rad);

    float degree_to_rad (float deg);
    float rad_to_degree (float rad);

    double random_real(double min, double max);
}

namespace ext::sf
{
    using Vector2d = ::sf::Vector2<double>;
}

#endif
