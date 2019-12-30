#include "RenderWindow.h"

#include "imgui/imgui.h"

namespace sfml_window
{
ImVec4 background_color = {0, 0, 0, 1};
sf::RenderWindow window {};
bool close_window = false;

void init_window()
{
    window.create(sf::VideoMode(default_window_size.x, default_window_size.y), "LSys");
    window.setVerticalSyncEnabled(true);
#ifdef _WIN32 // Window starts with wrong scaling, changing it like this stop this bug.
    window.setSize(sf::Vector2u(default_window_size.x - 1, default_window_size.y));
    window.setSize(sf::Vector2u(default_window_size.x, default_window_size.y));
#endif
}
} // namespace sfml_window
