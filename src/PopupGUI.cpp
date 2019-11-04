#include <iostream> // TODO REMOVE
#include <list>
#include "imgui_extension.h"
#include "PopupGUI.h"

namespace procgui
{
    // Incremented for each new popup.
    static int new_popup_id = 0;

    // Popup with its unique id.
    struct PopupEntry
    {
        PopupGUI popup;
        int id;
    };
    
    // The popup list.
    // Used as a stack for push/pop PopupGUI management, but iterating over it
    // is necessary to delete popups.
    std::list<PopupEntry> popups;

    
    void PopupGUI::operator()(sf::Keyboard::Key& key) const
    {
        ImGui::OpenPopup(name.c_str());
        if (ImGui::BeginPopupModal(name.c_str()))
        {
            // Close the popup
            if (only_info &&
                (key == sf::Keyboard::Escape ||
                 key == sf::Keyboard::Enter))
            {
                key = sf::Keyboard::Unknown;
                popups.pop_back();
            }

            if (message)
            {
                message();
            }

            // If the popup is more than just an informative message.
            if (!only_info)
            {
                if (key == sf::Keyboard::Enter)
                {
                    key = sf::Keyboard::Unknown;

                    // The order is important: 'ok_callback()' could add a
                    // callback on top of the stack.
                    popups.pop_back();
                    if (ok_callback)
                    {
                        ok_callback();
                    }
                }
                else if (key == sf::Keyboard::Escape)
                {
                    key = sf::Keyboard::Unknown;

                    popups.pop_back();
                    if (cancel_callback)
                    {
                        cancel_callback();
                    }
                }

                // Repetition but the 'key' here should stay the same
                ext::ImGui::PushStyleColoredButton<ext::ImGui::Green>();
                if (ImGui::Button(ok_text.c_str()))
                {
                    popups.pop_back();
                    if (ok_callback)
                    {
                        ok_callback();
                    }
                }
                ImGui::PopStyleColor(3);

                ImGui::SameLine();

                ext::ImGui::PushStyleColoredButton<ext::ImGui::Red>();
                if (ImGui::Button(cancel_text.c_str()))
                {
                    popups.pop_back();
                    if (cancel_callback)
                    {
                        cancel_callback();
                    }
                }
                ImGui::PopStyleColor(3);
            
            }

            ImGui::EndPopup();
        }
    }

    int push_popup (const PopupGUI& popup)
    {
        popups.push_back({popup, new_popup_id++});
        return popups.back().id;
    }

    void remove_popup(int id)
    {
        auto to_remove = std::find_if(begin(popups), end(popups),
                                      [id](const auto& e){return e.id == id;});

        if (to_remove != end(popups))
        {
            popups.erase(to_remove);
        }
    }

    bool popup_empty()
    {
        return popups.empty();
    }

    void display_popups (sf::Keyboard::Key& key)
    {
        if (popups.empty())
        {
            return;
        }

        // Do not put a const &: the popup will pop the top of stack, and it
        // will lead to danging reference.
        PopupGUI popup = popups.back().popup;
        popup(key);
    }
}
