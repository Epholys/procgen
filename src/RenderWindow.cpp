#include "RenderWindow.h"

namespace sfml_window
{
    sf::Color background_color = sf::Color::Black;
    sf::RenderWindow window {};
    
    void init_window()
    {
        window.create(sf::VideoMode(window_size.x, window_size.y), "Procgen");
        window.setVerticalSyncEnabled(true);
    }
}
