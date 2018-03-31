#ifndef LSYSTEM_CONTROLLER
#define LSYSTEM_CONTROLLER


#include "SFML/Graphics.hpp"
#include "LSystemView.h"

namespace controller
{
    void handle_input_views(std::vector<procgui::LSystemView>& views, const sf::Event& event);
}


#endif // LSYSTEM_CONTROLLER
