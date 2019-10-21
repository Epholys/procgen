#ifndef POPUP_GUI_H_N
#define POPUP_GUI_H_N

#include <string>
#include <functional>
#include <SFML/Window.hpp>
#include "imgui/imgui.h"

namespace procgui
{
    struct PopupGUI
    {
        std::string name {"Popup"};
        std::function<void()> message{nullptr};
        bool only_info { true };
        std::string ok_text = "OK";
        std::string cancel_text = "Cancel";
        std::function<void()> ok_callback {nullptr};

        void operator()(sf::Keyboard::Key &key) const;
    };
    
    void push_popup (const PopupGUI& popup);
    bool popup_empty();
    void display_popups (sf::Keyboard::Key& key);
}


#endif // POPUP_GUI_H_N
