#ifndef LSYSTEM_CONTROLLER
#define LSYSTEM_CONTROLLER


#include "SFML/Graphics.hpp"
#include "LSystemView.h"

namespace controller
{
    void handle_input_views(std::vector<procgui::LSystemView>& views, const sf::Event& event, const sf::View window_view, float zoom_level);
}


#endif // LSYSTEM_CONTROLLER
