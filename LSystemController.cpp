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
            // We want to have a specific behaviour : if a click is inside the
            // hitboxes of a LSystemView, we select it UNLESS an other view is
            // already selected at this click.
            auto to_select = views.end();
            for (auto it = views.begin(); it != views.end(); ++it)
            {
                if (it->is_inside(WindowController::real_mouse_position(
                                    {event.mouseButton.x,
                                     event.mouseButton.y})))
                {
                    // If the click is inside the hitboxes, select it... 
                    to_select = it;
                    if (it->is_selected())
                    {
                        // ... unless an other one is selected at this
                        // position. If that's the case stop the search.
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
