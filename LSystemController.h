#ifndef LSYSTEM_CONTROLLER
#define LSYSTEM_CONTROLLER


#include "SFML/Graphics.hpp"
#include "LSystemView.h"

namespace controller
{
    // LSystemController manages the behaviour of the LSystemViews beyond the
    // GUI: selection and dragging for example.
    //
    // This class is a singleton implemented as a static class.
    class LSystemController
    {
    public:
        // Delete the constructor to implement the singleton.
        LSystemController() = delete;

        // If the mouse if above a LSystemView ('under_mouse_'),
        // LSystemController has the priority.
        static bool has_priority();
        
        // Handle 'event' for the 'views'.
        static void handle_input(std::vector<procgui::LSystemView>& views, const sf::Event& event);

        // Handle the dragging behaviour.
        static void handle_delta(sf::Vector2f delta);

    private:
        // The LSystemView below the mouse. nullptr if there is
        // nothing. Non-owning pointer.
        static procgui::LSystemView* under_mouse_;
    };
}


#endif // LSYSTEM_CONTROLLER
