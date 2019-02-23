#ifndef RENDER_WINDOW_H
#define RENDER_WINDOW_H

#include <SFML/Graphics.hpp>

namespace window
{
    static constexpr struct WindowSize
    {
        int x {1600};
        int y {900};
    } window_size;

    extern sf::Color background_color;
}

#endif // RENDER_WINDOW_H
