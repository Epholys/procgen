#ifndef LSYSTEM_CONTROLLER
#define LSYSTEM_CONTROLLER


#include "SFML/Graphics.hpp"
#include "LSystemView.h"

namespace controller
{
    struct ctrl
    {
    
        procgui::LSystemView* under_mouse = nullptr;
    
        // Handle 'event' for the 'views'.
        void handle_input_views(std::vector<procgui::LSystemView>& views, const sf::Event& event);

        void handle_delta(sf::Vector2f delta);
    };
}


#endif // LSYSTEM_CONTROLLER
