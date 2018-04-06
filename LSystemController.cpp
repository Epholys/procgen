#include "LSystemController.h"
#include "WindowController.h"
#include "imgui/imgui.h"

namespace controller
{
    void handle_input_views(std::vector<procgui::LSystemView>& views, const sf::Event& event)
    {
        ImGuiIO& imgui_io = ImGui::GetIO();

        if (!imgui_io.WantCaptureMouse &&
            event.type == sf::Event::MouseButtonPressed &&
            event.mouseButton.button == sf::Mouse::Left)
        {
            auto to_select = views.end();
            for (auto it = views.begin(); it != views.end(); ++it)
            {
                if (it->is_inside(WindowController::real_mouse_position(
                                    {event.mouseButton.x,
                                     event.mouseButton.y})))
                {
                    to_select = it;
                    if (it->is_selected())
                    {
                        to_select = views.end();
                        break;
                    }

                }
            }
            if (to_select != views.end())
            {
                to_select->select();
            }
        }
    }
}
