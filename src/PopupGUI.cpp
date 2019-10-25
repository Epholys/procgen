#include <iostream> // TODO REMOVE
#include <stack>
#include "imgui_extension.h"
#include "PopupGUI.h"

namespace procgui
{
    // The popup stack
    std::stack<PopupGUI> popups;

    
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
                popups.pop();
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
                    popups.pop();
                    if (ok_callback)
                    {
                        ok_callback();
                    }
                }
                else if (key == sf::Keyboard::Escape)
                {
                    key = sf::Keyboard::Unknown;

                    popups.pop();
                    if (cancel_callback)
                    {
                        cancel_callback();
                    }
                }

                // Repetition but the 'key' here should stay the same
                ext::ImGui::PushStyleColoredButton<ext::ImGui::Green>();
                if (ImGui::Button(ok_text.c_str()))
                {
                    popups.pop();
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
                    popups.pop();
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

    void push_popup (const PopupGUI& popup)
    {
        popups.push(popup);
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
        PopupGUI popup = popups.top();
        popup(key);
    }
}
