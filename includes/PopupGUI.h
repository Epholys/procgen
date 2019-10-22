#ifndef POPUP_GUI_H_N
#define POPUP_GUI_H_N

#include <string>
#include <functional>
#include <SFML/Window.hpp>
#include "imgui/imgui.h"

namespace procgui
{
    // This file defines a struct and helper functions to be able to create
    // popups from anywhere in the code. Originally, popups were called only in
    // WindowController.h where it was easy to forward keyboard input, but the
    // addition of a popup in LSystemView for size warning created the need of a
    // more universal popup management.
    //
    // The usage of this framework is to construct a popup defining appopriate
    // attributes, then calls the 'push_popup()' to add it to the "hidden"
    // stack. In the part of the code managing events, 'display_popups()' should
    // be called with a sf::Keyboard::Key.
    
    // A struct describing a popup
    struct PopupGUI
    {
        // Window name of the popup
        std::string name {"Popup"};
        // Arbitrary function called every 'display_popups()'. Usually used to
        // display text and other widgets with imgui.
        std::function<void()> message{nullptr};
        // If true, the popup is a purely informative one, and no button with
        // callbacks will be displayed. 
        bool only_info { true };
        // The text of the "validate" button.
        std::string ok_text = "OK";
        // The text of the "cancel" button.
        std::string cancel_text = "Cancel";
        // Arbitrary function called when the popup is validated, meaning a
        // click on the 'ok_text' button or by pressing 'Enter'
        std::function<void()> ok_callback {nullptr};

        // Display the popup and defines its logic:
        //   - If 'key' is 'escape' or the cancel_button is clicked, the popup is
        //     closed.
        //   - If 'key' is 'enter' and
        //     - if 'only_info' is true, the popup is closed
        //     - otherwise, the 'ok_callback' is called and the popup is closed
        //   - If the ok_button is clicked, 'ok_callback' is called and the
        //     popup is closed
        //
        // If key is 'enter' or 'escape', it is set to 'sf::Keyboard::Unknown',
        // to avoid the other GUI elements to use it.
        //      
        // Closing a popup removes it from the popup stack.
        void operator()(sf::Keyboard::Key &key) const;
    };

    // Push 'popup' on top of the stack.
    void push_popup (const PopupGUI& popup);
    // Returns true if the stack is empty.
    bool popup_empty();
    // If the stack is non-empty, display the popup at the top of the
    // stack. Otherwise, do nothing.
    void display_popups (sf::Keyboard::Key& key);
}


#endif // POPUP_GUI_H_N
