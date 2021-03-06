#include "LSystemController.h"

#include "LSystemView.h"
#include "PopupGUI.h"
#include "WindowController.h"
#include "imgui/imgui.h"

namespace controller
{
procgui::LSystemView* LSystemController::under_mouse_ {nullptr};

std::optional<procgui::LSystemView> LSystemController::saved_view_ {};

const std::chrono::duration<unsigned long long, std::milli> LSystemController::double_click_time_ {
    std::chrono::milliseconds(300)};

std::chrono::time_point<std::chrono::steady_clock> LSystemController::click_time_ {};

bool LSystemController::has_priority()
{
    return under_mouse_ != nullptr;
}

const std::optional<procgui::LSystemView>& LSystemController::saved_view()
{
    return saved_view_;
}
procgui::LSystemView* LSystemController::under_mouse()
{
    return under_mouse_;
}

void LSystemController::handle_input(std::list<procgui::LSystemView>& views, const sf::Event& event)
{
    ImGuiIO& imgui_io = ImGui::GetIO();

    if (!imgui_io.WantCaptureMouse && event.type == sf::Event::MouseButtonPressed)
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
                    {event.mouseButton.x, event.mouseButton.y})))
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

    else if (!imgui_io.WantCaptureKeyboard && event.type == sf::Event::KeyPressed
             && event.key.code == sf::Keyboard::Delete)
    {
        // Remove the LSys under_mouse_ if there are several of them.
        if ((under_mouse_ != nullptr) && under_mouse_->is_selected())
        {
            delete_view(views, under_mouse_->get_id());
        }
        // If there is only one, delete it automatically
        else if (views.size() == 1)
        {
            delete_view(views, views.back().get_id());
        }
    }

    else if (!imgui_io.WantCaptureKeyboard && event.type == sf::Event::KeyPressed
             && (sf::Keyboard::isKeyPressed(sf::Keyboard::LControl)
                 || sf::Keyboard::isKeyPressed(sf::Keyboard::RControl))
             && under_mouse_ != nullptr && under_mouse_->is_selected())
    {
        if (event.key.code == sf::Keyboard::C)
        {
            saved_view_ = *under_mouse_;
        }
        else if (event.key.code == sf::Keyboard::S)
        {
            WindowController::open_save_menu();
        }
        else if (event.key.code == sf::Keyboard::E)
        {
            WindowController::open_export_menu();
        }
    }
}

void LSystemController::handle_delta(sf::Vector2f delta)
{
    if (under_mouse_ != nullptr)
    {
        auto& parameters = under_mouse_->ref_parameters();
        auto starting_position = parameters.get_starting_position() - ext::sf::Vector2d(delta);
        parameters.set_starting_position(starting_position);
    }
}

void LSystemController::delete_view(std::list<procgui::LSystemView>& views, int id)
{
    auto to_delete = std::find_if(begin(views), end(views), [id](const auto& v) {
        return v.get_id() == id;
    });
    Expects(to_delete != end(views));

    if (to_delete->is_modified())
    {
        procgui::PopupGUI save_warning_popup = {
            "Save Warning##LSysController",
            []() {
                ImGui::Text("This L-System has unsaved modifications, are you sure to delete it?");
            },
            false,
            "Yes",
            "No",
            [to_delete, &views]() {
                views.erase(to_delete);
                under_mouse_ = nullptr;
            }};

        procgui::push_popup(save_warning_popup);
    }
    else
    {
        views.erase(to_delete);
        under_mouse_ = nullptr;
    }
}

void LSystemController::right_click_menu(std::list<procgui::LSystemView>& views)
{
    if (ImGui::BeginPopupContextVoid())
    {
        // Cloning is deep-copying the LSystem.
        if (ImGui::MenuItem("Copy", "Ctrl+C") && (under_mouse_ != nullptr))
        {
            saved_view_ = *under_mouse_;
        }
        if (ImGui::MenuItem("Delete", "Del") && (under_mouse_ != nullptr))
        {
            delete_view(views, under_mouse_->get_id());
        }
        ImGui::Separator();
        if (ImGui::MenuItem("Save", "Ctrl+S"))
        {
            WindowController::open_save_menu();
        }
        else if (ImGui::MenuItem("Export", "Ctrl+E"))
        {
            WindowController::open_export_menu();
        }
        ImGui::EndPopup();
    }
}
} // namespace controller
