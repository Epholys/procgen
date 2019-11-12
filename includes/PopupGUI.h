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
    // stack-like structure. In the part of the code managing events,
    // 'display_popups()' should be called with a sf::Keyboard::Key.
    //
    // A PopupGUI can run arbitrary code, by using std::function<>. So,
    // 'remove_popup()' is necessary if this arbitrary code has reference to
    // deleted objects. This is a manual memory management-like way of doing
    // things, but that's a consequence of the flexibility of the popups.

    // A struct describing a popup
    struct PopupGUI
    {
        // Window name of the popup
        std::string name {"Popup"};
        // Arbitrary function called every 'display_popups()'. Usually used to
        // display text and other widgets with imgui.
        std::function<void()> message{nullptr};
        // If true, the popup is a purely informative one, so no cancel button
        // with be displayed.
        bool only_info { true };
        // The text of the "validate" button.
        std::string ok_text = "OK";
        // The text of the "cancel" button.
        std::string cancel_text = "Cancel";
        // Arbitrary function called when the popup is validated, meaning a
        // click on the 'ok_text' button or by pressing 'Enter', if 'only_info'
        // is false.
        std::function<void()> ok_callback {nullptr};
        // Arbitrary function called when the popup is cancelled, meaning a
        // click on the 'cancel_text' button or by pressing 'Escape', if
        // 'only_info' is false.
        std::function<void()> cancel_callback {nullptr};

        // Display the popup and defines its logic:
        //   - If 'key' is 'enter' or the ok_button is clicked, 'ok_callback' is
        //   called, if it is defined, and the popup us closed.
        //   - If the popup is a 'only_info': no cancel_button is displayed, and
        //   if the 'key' is 'escape', the popup is closed without any
        //   callback.
        //   - Otherwise, a cancel_button is displayed, and if the user clicks
        //   on it, or if 'key' is 'escape', the 'cancel_callback' is called,
        //   if defined, and the popup is closed.
        //
        // If key is 'enter' or 'escape', it is set to 'sf::Keyboard::Unknown',
        // to avoid the other GUI elements to use it.
        //
        // Closing a popup removes it from the popup stack.
        void operator()(sf::Keyboard::Key &key) const;
    };

    // Push 'popup' on top of the stack.
    // Returns the unique id of 'popup' in the underlying data structure.
    int push_popup (const PopupGUI& popup);
    // Remove the popup with unique id 'id' from the data structure.
    // Do nothing if no popup with id 'id' exists.
    void remove_popup (int id);
    // Returns true if the stack is empty.
    bool popup_empty();
    // If the stack is non-empty, display the popup at the top of the
    // stack. Otherwise, do nothing.
    void display_popups (sf::Keyboard::Key& key);
}


#endif // POPUP_GUI_H_N
