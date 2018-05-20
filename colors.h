#ifndef COLORS_H
#define COLORS_H


#include <unordered_map>
#include <vector>
#include <SFML/Graphics/Color.hpp>

namespace colors
{
    class UniqueColor
    {
    public:
        sf::Color register_id(int id);
        void remove_id(int id);
        sf::Color get_color(int id);

    private:
        sf::Color new_color();

        std::vector<sf::Color> color_pool_ {};
        std::unordered_map<int /*index*/, int /*identifier*/> map_ {};
        int iter_ {0};
        int pass_ {0};
    };
}


#endif // COLORS_HPP
