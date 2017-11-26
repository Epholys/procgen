#ifndef HELPER_MATH_H
#define HELPER_MATH_H

// This namespace defines commonly used function and constants missing in <cmath>.
namespace math
{
    constexpr double pi = 3.14159;

    constexpr double degree_to_rad (double deg) { return deg * pi / 180; }
    constexpr double rad_to_degree (double rad) { return rad * 180 / pi; }

    constexpr float degree_to_rad (float deg) { return deg * pi / 180; }
    constexpr float rad_to_degree (float rad) { return rad * 180 / pi; }
}

#endif
