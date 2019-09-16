#ifndef HELPER_MATH_H
#define HELPER_MATH_H

#include <random>
#include <cmath>
#include <SFML/System.hpp>

// This namespace defines commonly used function and constants missing in <cmath>.
namespace math
{
    static constexpr double pi = 3.141592653589793;
    static std::random_device random_dev;
    static constexpr double double_max_limit = 1e8;


    template<typename N>
    constexpr N degree_to_rad (N deg)
    {
        static_assert(std::is_arithmetic<N>::value, "degree_to_rad() must be called with an arithmetic argument");

        return deg * pi / 180;
    }
    template<typename N>
    constexpr N rad_to_degree (N rad)
    {
        static_assert(std::is_arithmetic<N>::value, "rad_to_degree() must be called with an arithmetic argument");

        return rad * 180 / pi;
    }

    template<typename Generator>
    double random_real(Generator& gen, double min, double max)
    {
        std::uniform_real_distribution<> dis(min, max);
        return dis(gen);
    }

    template<typename T>
    constexpr const T& clamp_angle(const T& val)
    {
        T min = 0;
        T max = 360;
        return std::clamp(val, min, max);
    }
}

namespace ext::sf
{
    using Vector2d = ::sf::Vector2<double>;
}

#endif
