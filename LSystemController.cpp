#include "LSystemController.h"
#include "WindowController.h"
#include "imgui/imgui.h"

namespace controller
{
    procgui::LSystemView* LSystemController::under_mouse_ {nullptr};
    
    std::optional<procgui::LSystemView> LSystemController::saved_view_ {};

    const std::chrono::duration<unsigned long long, std::milli> LSystemController::double_click_time_
    {std::chrono::milliseconds(300)};

    std::chrono::time_point<std::chrono::steady_clock> LSystemController::click_time_ {};

    
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
            // We have a double-click when the previous click is closer to the
            // present than 'double_click_time_'.
            bool double_click = false;
            if (event.mouseButton.button == sf::Mouse::Left)
            {
                if (std::chrono::steady_clock::now() - click_time_ < double_click_time_)
                {
                    double_click = true;
                }
                click_time_ = std::chrono::steady_clock::now();
            }
            
            // We want to have a specific behaviour : if a click is inside the
            // hitboxes of a LSystemView, we select it for 'under_mouse_' UNLESS
            // an other view is already selected at this click.
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

                if (double_click)
                {
                    to_select->select();
                }
            }
            else if (!already_selected)
            {
                under_mouse_ = nullptr;
            }
        }

        else if (!imgui_io.WantCaptureKeyboard &&
                 event.type == sf::Event::KeyPressed &&
                 (sf::Keyboard::isKeyPressed(sf::Keyboard::LControl) ||
                  sf::Keyboard::isKeyPressed(sf::Keyboard::RControl)) &&
                 under_mouse_ != nullptr &&
                 under_mouse_->is_selected())
        {
            if (event.key.code == sf::Keyboard::C)
            {
                saved_view_ = under_mouse_->clone();
            }
            else if (event.key.code == sf::Keyboard::X)
            {
                saved_view_ = under_mouse_->duplicate();
            }

        }

    }

    void LSystemController::handle_delta(sf::Vector2f delta)
    {
        if (under_mouse_)
        {
            auto& parameters = under_mouse_->ref_parameters();
            parameters.starting_position -= delta;
            under_mouse_->compute_vertices();
        }
    }

    void LSystemController::right_click_menu()
    {
        if (ImGui::BeginPopupContextVoid())
        {
            // Cloning is deep-copying the LSystem.
            if (ImGui::MenuItem("Clone", "Ctrl+C"))
            {
                saved_view_ = under_mouse_->clone();
            }
            if (ImGui::MenuItem("Duplicate", "Ctrl+X"))
            {
                saved_view_ = under_mouse_->duplicate();
            }
            ImGui::EndPopup();
        }
    }
}
