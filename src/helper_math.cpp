#include "helper_math.h"

namespace math
{
    double degree_to_rad (double deg) { return deg * pi / 180; }
    double rad_to_degree (double rad) { return rad * 180 / pi; }

    float degree_to_rad (float deg) { return deg * pi / 180; }
    float rad_to_degree (float rad) { return rad * 180 / pi; }

    double random_real(double min, double max)
    {
        std::uniform_real_distribution<> dis(min, max);
        return dis(random_generator);
    }

}
