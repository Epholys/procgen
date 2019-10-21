#include <iostream> // TODO REMOVE
#include <stack>
#include "imgui_extension.h"
#include "PopupGUI.h"

namespace procgui
{
    std::stack<PopupGUI> popups;

    void PopupGUI::operator()(sf::Keyboard::Key& key) const
    {
        ImGui::OpenPopup(name.c_str());
        if (ImGui::BeginPopupModal(name.c_str()))
        {
            if (key == sf::Keyboard::Escape ||
                (only_info && key == sf::Keyboard::Enter))
            {
                key = sf::Keyboard::Unknown;
                popups.pop();
            }

            if (message)
            {
                message();
            }

            if (!only_info)
            {
                if (key == sf::Keyboard::Enter)
                {
                    key = sf::Keyboard::Unknown;
                    popups.pop(); // before: callback can open popup
                    if (ok_callback)
                    {
                        ok_callback();
                    }
                }
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

        // NOT const auto&
        auto popup = popups.top();
        popup(key);
    }
}
