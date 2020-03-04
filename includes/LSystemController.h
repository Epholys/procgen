#ifndef LSYSTEM_CONTROLLER
#define LSYSTEM_CONTROLLER


#include "SFML/Graphics.hpp"

#include <chrono>
#include <gsl/gsl>
#include <list>
#include <optional>

namespace procgui
{
class LSystemView;
}

namespace controller
{
// LSystemController manages the behaviour of the LSystemViews beyond the
// GUI: selection, dragging and riglt-click for example.
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
    static void handle_input(std::list<procgui::LSystemView>& views, const sf::Event& event);

    // Handle the dragging behaviour.
    static void handle_delta(sf::Vector2f delta);

    // Interact with a menu when right-clicking on a LSystemView (cloning,
    // duplicating, ...)
    static void right_click_menu(std::list<procgui::LSystemView>& views);

    // Getters
    static const std::optional<procgui::LSystemView>& saved_view();
    static procgui::LSystemView* under_mouse();

    // Set the under_mouse_ attribute to an existing LSystemView.
    // Used when loading a LSystem to have it already selected for all usual operations.
    // Please be careful with obejct lifetime management.
    static void set_under_mouse(gsl::not_null<procgui::LSystemView*> under_mouse);

  private:
    // Delete the LSystemView with identifier 'id' in 'views'
    static void delete_view(std::list<procgui::LSystemView>& views, int id);

    // The LSystemView below the mouse. nullptr if there is
    // nothing. Non-owning pointer.
    static procgui::LSystemView* under_mouse_;

    // When copying or duplicating in a right-click, we save the LSystemView
    // in this variable. optional<> is used to initialize an empty variable
    // when starting up the application.
    static std::optional<procgui::LSystemView> saved_view_;

    // The delay between two click before considering a double-click. Based
    // on the imgui time.
    static const std::chrono::duration<unsigned long long, std::milli> double_click_time_;
    // Previous left-click timestamp.
    static std::chrono::time_point<std::chrono::steady_clock> click_time_;
};
} // namespace controller


#endif // LSYSTEM_CONTROLLER
