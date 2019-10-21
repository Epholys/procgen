#ifndef WINDOW_CONTROLLER_H
#define WINDOW_CONTROLLER_H


#include <list>
#include <experimental/filesystem>

#include <SFML/Graphics.hpp>

#include "imgui/imgui.h"

#include "SaveMenu.h"

namespace procgui
{
    class LSystemView;
}

namespace controller
{
    // WindowController is the hub for all the inputs. The main code in
    // 'handle_input()' manages the 'sf::View' of the window: resizing, zooming, and
    // dragging the view. All the others component of this application needing input
    // (imgui, *Views) are called in this class but managed elsewhere.
    //
    // WindowController is a singleton implemented as a static class.
    //
    // TODO: this class is becoming a little too big, managing not only inputs
    // but saving and loading windows. That should put elsewhere.
    class WindowController
    {
    public:
        // Delete the constructors to implement the singleton.
        WindowController() = delete;

        // Hub of all the input of the application, manages locally the 'sf::View'
        // of the 'window'.
        static void handle_input(std::vector<sf::Event> events,
                                 std::list<procgui::LSystemView>& lsys_views);

        // The 'sf::Mouse::getPosition()' give the absolute position in a
        // window. This method get the mouse position with the application
        // coordinates relative to the drawing of the application.
        static sf::Vector2f real_mouse_position(sf::Vector2i mouse_click);

        // Inverse operation of 'real_mouse_position()' : from a relative mouse
        // position in the application coordinates, returns the absolution
        // position in the window coordinate.
        static sf::Vector2i absolute_mouse_position(sf::Vector2f mouse_click);
        
        // Get the absolute mouse position
        static sf::Vector2i get_mouse_position();

        // Getter for the zoom level
        static float get_zoom_level();

        // Add a error message to be displayed at loading time.
        // This function should be called when an error occurs when
        // deserializing a LSystemView.
        static void add_loading_error_message(const std::string& message);
        
        // Public flag to message WindowController to start the save menu. It is
        // also used to let the window open between frames.
        // The save menu is managed in this class with the load menu, but can be
        // called elsewhere, so there is this ugly flag.
        static bool save_menu_open_;

    private:
        // Helper method to paste 'view' at 'position' and add it to
        // 'lsys_views'. 
        static void paste_view(std::list<procgui::LSystemView>& lsys_views,
                               const std::optional<procgui::LSystemView>& view,
                               const sf::Vector2f& position);

        // The right-click menu managing everything between
        // creation/copy-pasting of LSystemViews, saving and loading.
        static void right_click_menu(sf::RenderWindow& window,
                                     std::list<procgui::LSystemView>& lsys_view);

        // Display and interact with the save menu window.
        // Managed the window, opening and saving into a file.
        // static void save_menu(sf::Keyboard::Key key  = sf::Keyboard::Unknown);
        static SaveMenu save_menu_;
        
        // Private flag to let the load menu open between frames.
        static bool load_menu_open_;

        // Display and interact with the load menu window. Load from files the
        // LSystems into 'lsys_views'. 'key' is forwarded to manage navigation
        // and 'unicode' to help selection.
        static void load_menu(std::list<procgui::LSystemView>& lsys_views,
                              sf::Keyboard::Key key = sf::Keyboard::Unknown,
                              sf::Uint32 unicode = 0);

        // Flag to let the quit popup open;
        static bool quit_popup_open_;
        // If at least one open LSystemView is not saved, open a quit warning popup.
        static void quit_popup(sf::Keyboard::Key key);

        // When ordering the load menu to open, save the current mouse position
        // to load the LSystemView at this position instead of the center.
        static sf::Vector2f mouse_position_to_load_;

        
        // The view modified by the user and given to the window.
        static sf::View view_;

        // The zoom level in the window.
        static float zoom_level_;
        // Default step size at default zoom level
        static const double default_step_;
        
        // Current mouse position
        static sf::Vector2i mouse_position_;

        // Does the window have the focus?
        static bool has_focus_;

        // Signal that the view can be dragged.
        static bool view_can_move_;

        // Arbitrary value for the maximum length of the file name.
        static constexpr int FILENAME_LENGTH_ = 128;
        
        // The fixed save directory of the application
        static std::experimental::filesystem::path save_dir_;

        // Error messages to be displayed if necessary at loading time.
        static std::vector<std::string> error_messages;
    };
}

#endif
