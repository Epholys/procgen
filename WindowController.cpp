#include <fstream>
#include <memory>
#include <experimental/filesystem>

#include "imgui/imgui.h"
#include "imgui/imgui-SFML.h"
#include "cereal/archives/json.hpp"

#include "helper_string.h"
#include "WindowController.h"
#include "LSystemController.h"

namespace fs = std::experimental::filesystem;

namespace controller
{
    sf::View WindowController::view_ {};

    float WindowController::zoom_level_ {1.f};

    sf::Vector2i WindowController::mouse_position_ {};
    
    bool WindowController::has_focus_ {true};

    bool WindowController::view_can_move_ {false};

    bool WindowController::save_window_open_ {false};

    
    sf::Vector2f WindowController::real_mouse_position(sf::Vector2i mouse_click)
    {
        auto size = view_.getSize();
        auto center = view_.getCenter();
        sf::Vector2f upright {center.x - size.x/2, center.y - size.y/2};
        sf::Vector2f position {mouse_click.x*zoom_level_ + upright.x,
                               mouse_click.y*zoom_level_ + upright.y};
        return position;
    }

    void WindowController::paste_view(sf::RenderWindow& window, std::vector<procgui::LSystemView>& lsys_views)
    {
        if (!LSystemController::saved_view())
        {
            return;
        }
        const auto& saved = LSystemController::saved_view();

        // Before adding the view to the vector<>, update
        // 'starting_position' to the new location.
        auto view = *saved;
        auto box = view.get_bounding_box();
        sf::Vector2f middle = {box.left + box.width/2, box.top + box.height/2};
        middle = view.get_parameters().starting_position - middle;
        view.ref_parameters().starting_position = real_mouse_position(sf::Mouse::getPosition(window)) + middle;
        view.compute_vertices();
        lsys_views.emplace_back(view);

    }
    
    void WindowController::right_click_menu(sf::RenderWindow& window, std::vector<procgui::LSystemView>& lsys_views)
    {
        if (ImGui::BeginPopupContextVoid())
        {
            if (ImGui::MenuItem("New LSystem", "Ctrl+N"))
            {
                lsys_views.emplace_back(real_mouse_position(sf::Mouse::getPosition(window)));
            }
            ImGui::Separator();
            if (LSystemController::saved_view() && ImGui::MenuItem("Paste", "Ctrl+V"))
            {
                paste_view(window, lsys_views);
            }
            ImGui::EndPopup();
        }
    }

    void WindowController::save_window()
    {
        const std::string popup_name = "Save LSystem to file";
        static std::array<char, 64> filename;
        ImGui::OpenPopup(popup_name.c_str());
        if (ImGui::BeginPopupModal(popup_name.c_str(), &save_window_open_, ImGuiWindowFlags_AlwaysAutoResize))
        {
            ImGui::Separator();
            fs::path save_dir = fs::u8path(u8"saves");
            try
            {
                for (const auto& file : fs::directory_iterator(save_dir))
                {
                    if (fs::is_regular_file(file.path()) &&
                        ImGui::Selectable(file.path().filename().c_str()))
                    {
                        filename = string_to_array<filename.size()>(file.path().filename());
                    }
                }
            }
            catch (const fs::filesystem_error& e)
            {
                ImGui::OpenPopup("Error");
                if (ImGui::BeginPopupModal("Error"))
                {
                    ImGui::Text(e.what());
                    ImGui::EndPopup();
                }
            }

            ImGui::Separator();

            ImGui::CaptureKeyboardFromApp();
            ImGui::InputText("Filename", filename.data(), filename.size());
            
            ImGui::Separator();

            static bool open_error_popup = false;
            if (ImGui::Button("Save"))
            {
                std::ofstream ofs (save_dir/array_to_string(filename));

                if(!ofs.is_open())
                {
                    open_error_popup = true;
                }
                else
                {
                    cereal::JSONOutputArchive archive (ofs);
                    if (LSystemController::under_mouse())
                    {
                        archive(*LSystemController::under_mouse());
                    }
                        save_window_open_ = false;
                }
            }

            if (open_error_popup)
            {
                ImGui::OpenPopup("Error");
                if (ImGui::BeginPopupModal("Error", &open_error_popup))
                {
                    std::string message = "Error: can't open file: " + array_to_string(filename);
                    ImGui::Text(message.c_str());
                    ImGui::EndPopup();
                }
            }

            ImGui::SameLine();
            if (ImGui::Button("Cancel"))
            {
                save_window_open_ = false;
            }
            
            ImGui::EndPopup();
        }
    }

    
    void WindowController::handle_input(sf::RenderWindow &window, std::vector<procgui::LSystemView>& lsys_views)
    {
        ImGuiIO& imgui_io = ImGui::GetIO();
        sf::Event event;

        while (window.pollEvent(event))
        {
            // ImGui has the priority as it is the topmost GUI.
            ImGui::SFML::ProcessEvent(event);

            // Close the Window if necessary
            if (event.type == sf::Event::Closed ||
                (!imgui_io.WantCaptureKeyboard &&
                 event.type == sf::Event::KeyPressed &&
                 event.key.code == sf::Keyboard::Escape))
            {
                window.close();
            }


            else if (!imgui_io.WantCaptureKeyboard &&
                     event.type == sf::Event::KeyPressed &&
                     (sf::Keyboard::isKeyPressed(sf::Keyboard::LControl) ||
                      sf::Keyboard::isKeyPressed(sf::Keyboard::RControl)))
            {
                if (event.key.code == sf::Keyboard::V)
                {
                    paste_view(window, lsys_views);
                }
                else if (event.key.code == sf::Keyboard::N)
                {
                    lsys_views.emplace_back(real_mouse_position(sf::Mouse::getPosition(window)));
                }
                                
            }

            else if (event.type == sf::Event::GainedFocus)
            {
                has_focus_ = true;
                // Note: the view_ can not move yet: the old position of the mouse is
                // still in memory, it must be updated.
            }
            else if (event.type == sf::Event::LostFocus)
            {
                has_focus_ = false;
                view_can_move_ = false;
            }
            else if (event.type == sf::Event::Resized)
            {
                view_.setSize(event.size.width, event.size.height);
            }

            else if (has_focus_)
            {
                if(!imgui_io.WantCaptureMouse &&
                   event.type == sf::Event::MouseWheelMoved)
                {
                    // Adjust the zoom level
                    auto delta = event.mouseWheel.delta;
                    if (delta>0)
                    {
                        zoom_level_ *= 0.9f;
                        view_.zoom(0.9f);
                    }
                    else if (delta<0)
                    {
                        zoom_level_ *= 1.1f;
                        view_.zoom(1.1f);
                    }
                }

                if (event.type == sf::Event::MouseButtonPressed &&
                    event.mouseButton.button == sf::Mouse::Left)
                {
                    // Update the mouse position and finally signal that the view_
                    // can now move.
                    // Note: the mouse position does not need to be relative to
                    // the drawing, so real_mouse_position() is not necessary.
                    mouse_position_ = sf::Mouse::getPosition(window);
                    view_can_move_ = true;
                }
            }
            
            LSystemController::handle_input(lsys_views, event);
        }

        if (save_window_open_)
        {
            save_window();
        }

        // The right-click menu depends on the location of the mouse.
        if (LSystemController::has_priority())
        {
            LSystemController::right_click_menu();
        }
        else
        {
            right_click_menu(window, lsys_views);
        }

        // Dragging behaviour
        if (has_focus_ && view_can_move_ &&
            !imgui_io.WantCaptureMouse &&
            sf::Mouse::isButtonPressed(sf::Mouse::Left))
        {
            sf::Vector2i new_position = sf::Mouse::getPosition(window);
            sf::IntRect window_rect (sf::Vector2i(0,0), sf::Vector2i(window.getSize()));
            if (window_rect.contains(new_position))
            {
                sf::Vector2i mouse_delta = mouse_position_ - new_position;
                if (LSystemController::has_priority())
                {
                    // If LSystemView management has priority, let them have the
                    // control over the dragging behaviour.
                    LSystemController::handle_delta(sf::Vector2f(mouse_delta) * zoom_level_);
                }
                else
                {
                    view_.move(sf::Vector2f(mouse_delta) * zoom_level_);
                }
                mouse_position_ = new_position;
            }
        }
    
        window.setView(view_);
    }
}
