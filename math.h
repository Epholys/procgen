#ifndef MATH_H
#define MATH_H


namespace math
{
    const double pi = 3.14159;

    inline double degree_to_rad (double deg) { return deg * pi / 180; }
    inline double rad_to_degree (double rad) { return rad * 180 / pi; }
}

#endif
