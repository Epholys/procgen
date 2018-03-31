#ifndef WINDOW_CONTROLLER_H
#define WINDOW_CONTROLLER_H


#include <SFML/Graphics.hpp>

// WindowController is the hub for all the inputs. The main code in
// 'handle_input()' manages the 'sf::View' of the window: resizing, zooming, and
// dragging the view. All the others component of this application needing input
// (imgui, *Views) are called in this call but managed elsewhere.
class WindowController
{
public:
    // Just to signal that there is a default constructor.
    WindowController() = default;

    // Hub of all the input of the application, manages locally the 'sf::View'
    // of the 'window'.
    void handle_input(sf::RenderWindow& window);
    
private:
    // Current zoom level (useful for dragging)
    float zoom_level_ {1.f};

    // Current mouse position
    sf::Vector2i mouse_position_ {};

    // Does the window have the focus?
    bool has_focus_ {true};

    // Signal that the view can be dragged.
    bool view_can_move_ {false};
};


#endif
