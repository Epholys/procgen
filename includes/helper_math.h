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

    double degree_to_rad (double deg);
    double rad_to_degree (double rad);

    float degree_to_rad (float deg);
    float rad_to_degree (float rad);

    template<typename Generator>
    double random_real(Generator& gen, double min, double max)
    {
        std::uniform_real_distribution<> dis(min, max);
        return dis(gen);
    }
    
}

namespace ext::sf
{
    using Vector2d = ::sf::Vector2<double>;
}

#endif
