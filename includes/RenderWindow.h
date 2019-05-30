#ifndef RENDER_WINDOW_H
#define RENDER_WINDOW_H

#include <SFML/Graphics.hpp>

namespace sfml_window
{
    static constexpr struct WindowSize
    {
        int x {1600};
        int y {900};
    } default_window_size;

    extern sf::Color background_color;

    void init_window();
    extern sf::RenderWindow window;
}

#endif // RENDER_WINDOW_H
