#include "imgui/imgui.h"
#include "RenderWindow.h"

namespace sfml_window
{
    ImVec4 background_color = {0, 0, 0, 1};
    sf::RenderWindow window {};
    bool close_window = false;

    void init_window()
    {
        window.create(sf::VideoMode(default_window_size.x, default_window_size.y), "Procgen");
        window.setVerticalSyncEnabled(true);
    }
}
