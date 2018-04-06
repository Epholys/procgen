#ifndef WINDOW_CONTROLLER_H
#define WINDOW_CONTROLLER_H


#include <vector>
#include <SFML/Graphics.hpp>
#include "LSystemView.h"

namespace controller
{
    // WindowController is the hub for all the inputs. The main code in
    // 'handle_input()' manages the 'sf::View' of the window: resizing, zooming, and
    // dragging the view. All the others component of this application needing input
    // (imgui, *Views) are called in this call but managed elsewhere.
    class WindowController
    {
    public:
        // Just to signal that there is a default constructor.
        WindowController() = delete;

        // Hub of all the input of the application, manages locally the 'sf::View'
        // of the 'window'.
        static void handle_input(sf::RenderWindow& window, std::vector<procgui::LSystemView>& lsys_views);

        static sf::Vector2f real_mouse_position(sf::Vector2i mouse_click);
    
    private:
        static sf::View view_;

        static float zoom_level_;

        // Current mouse position
        static sf::Vector2i mouse_position_;

        // Does the window have the focus?
        static bool has_focus_;

        // Signal that the view can be dragged.
        static bool view_can_move_;
    };
}

#endif
