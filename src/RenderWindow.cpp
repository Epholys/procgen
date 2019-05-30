#include "RenderWindow.h"

namespace sfml_window
{
    sf::Color background_color = sf::Color::Black;
    sf::RenderWindow window {};
    
    void init_window()
    {
        window.create(sf::VideoMode(default_window_size.x, default_window_size.y), "Procgen");
        window.setVerticalSyncEnabled(true);
    }
}
