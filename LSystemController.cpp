#include "LSystemController.h"
#include "WindowController.h"
#include "imgui/imgui.h"

namespace controller
{
    procgui::LSystemView* LSystemController::under_mouse_ {nullptr};
    
    std::optional<procgui::LSystemView> LSystemController::saved_view_ {};
    
    bool LSystemController::has_priority()
    {
        return under_mouse_ != nullptr;
    }

    const std::optional<procgui::LSystemView>& LSystemController::saved_view()
    {
        return saved_view_;
    }

    
    void LSystemController::handle_input(std::vector<procgui::LSystemView>& views, const sf::Event& event)
    {
        ImGuiIO& imgui_io = ImGui::GetIO();

        if (!imgui_io.WantCaptureMouse &&
            event.type == sf::Event::MouseButtonPressed)
        {
            // We want to have a specific behaviour : if a click is inside the
            // hitboxes of a LSystemView, we select it UNLESS an other view is
            // already selected at this click.
            auto to_select = views.end();
            bool already_selected = false;
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
                        under_mouse_ = &(*it);
                        already_selected = true;

                        // ... unless an other one is selected at this
                        // position. If that's the case stop the search.
                        to_select = views.end();
                        break;
                    }

                }
            }
            if (to_select != views.end())
            {
                under_mouse_ = &(*to_select);

                if (event.mouseButton.button == sf::Mouse::Left)
                {
                    to_select->select();
                }
            }
            else if (!already_selected)
            {
                under_mouse_ = nullptr;
            }
        }
    }

    void LSystemController::handle_delta(sf::Vector2f delta)
    {
        if (under_mouse_)
        {
            auto& parameters = under_mouse_->get_parameters();
            parameters.starting_position -= delta;
            under_mouse_->compute_vertices();
        }
    }

    void LSystemController::right_click_menu()
    {
        if (ImGui::BeginPopupContextVoid())
        {
            if (ImGui::MenuItem("Clone"))
            {
                saved_view_ = under_mouse_->clone();
            }
            ImGui::EndPopup();
        }
    }
}
