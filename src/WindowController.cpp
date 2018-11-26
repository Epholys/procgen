#include <fstream>

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
    sf::Vector2f WindowController::mouse_position_to_load_;
    
    bool WindowController::has_focus_ {true};

    bool WindowController::view_can_move_ {false};

    bool WindowController::save_menu_open_ {false};
    bool WindowController::load_menu_open_ {false};

    fs::path WindowController::save_dir_ = fs::u8path(u8"saves");
    
    sf::Vector2f WindowController::real_mouse_position(sf::Vector2i mouse_click)
    {
        auto size = view_.getSize();
        auto center = view_.getCenter();
        sf::Vector2f upright {center.x - size.x/2, center.y - size.y/2};
        sf::Vector2f position {mouse_click.x*zoom_level_ + upright.x,
                               mouse_click.y*zoom_level_ + upright.y};
        return position;
    }

    sf::Vector2i WindowController::absolute_mouse_position(sf::Vector2f mouse_click)
    {
        auto size = view_.getSize();
        auto center = view_.getCenter();
        sf::Vector2f upright {center.x - size.x/2, center.y - size.y/2};
        sf::Vector2i position((mouse_click.x - upright.x) / zoom_level_,
                              (mouse_click.y - upright.y) * zoom_level_);
        return position;
    }

    
    sf::Vector2i WindowController::get_mouse_position()
    {
        return mouse_position_;
    }


    void WindowController::paste_view(std::list<procgui::LSystemView>& lsys_views,
                                      const std::optional<procgui::LSystemView>& view,
                                      const sf::Vector2f& position)
    {
        if (!view)
        {
            return;
        }

        // Before adding the view to the vector<>, update
        // 'starting_position' to the new location.
        const auto& pasted_view = *view;
        auto box = pasted_view.get_bounding_box();
        ext::sf::Vector2d pos {position};
        ext::sf::Vector2d middle = {box.left + box.width/2, box.top + box.height/2};
        middle = pasted_view.get_parameters().get_starting_position() - middle;
        if (LSystemController::is_clone())
        {
            auto cloned_view = pasted_view.clone();
            cloned_view.ref_parameters().set_starting_position(pos + middle);
            lsys_views.emplace_front(cloned_view);
        }
        else
        {
            auto duplicated_view = pasted_view.duplicate();
            duplicated_view.ref_parameters().set_starting_position(pos + middle);
            lsys_views.emplace_front(duplicated_view);
        }
    }
    
    void WindowController::right_click_menu(sf::RenderWindow& window, std::list<procgui::LSystemView>& lsys_views)
    {
        if (ImGui::BeginPopupContextVoid())
        {
            if (ImGui::MenuItem("New LSystem", "Ctrl+N"))
            {
                lsys_views.emplace_front(ext::sf::Vector2d(real_mouse_position(sf::Mouse::getPosition(window))));
            }
            if (ImGui::MenuItem("Load LSystem", "Ctrl+O"))
            {
                mouse_position_to_load_ = real_mouse_position(sf::Mouse::getPosition(window));
                load_menu_open_ = true;
            }
            ImGui::Separator();
            if (LSystemController::saved_view() && ImGui::MenuItem("Paste", "Ctrl+V"))
            {
                paste_view(lsys_views, LSystemController::saved_view(), real_mouse_position(sf::Mouse::getPosition(window)));
            }
            ImGui::EndPopup();
        }
    }

    void WindowController::save_menu()
    {
        // The file name in which will be save the LSystem.
        static std::array<char, FILENAME_LENGTH_> filename;
        // Flag to let the directory error popup open between frames.
        static bool dir_error_popup = false;
        // Flag to let the file error popup open between frames.
        static bool file_error_popup = false;

        ImGui::SetNextWindowPosCenter();
        if (ImGui::Begin("Save LSystem to file", &save_menu_open_, ImGuiWindowFlags_AlwaysAutoResize|ImGuiWindowFlags_NoCollapse|ImGuiWindowFlags_NoSavedSettings))
        {
            // Avoid interaction with the background when saving a file.
            ImGui::CaptureKeyboardFromApp();
            ImGui::CaptureMouseFromApp();

            ImGui::Separator();
            try
            {
                // For each file of the 'save_dir_' directory...
                for (const auto& file : fs::directory_iterator(save_dir_))
                { 
                    // ... displays it in a selectable list ...
                   if (fs::is_regular_file(file.path()) &&
                        ImGui::Selectable(file.path().filename().c_str()))
                    {
                        // ... and set 'filename' to it when clicked (to overwrite this file).
                        filename = string_to_array<filename.size()>(file.path().filename());
                    }
                }
            }
            catch (const fs::filesystem_error& e)
            {
                // If we can't open 'save_dir_', open an error popup.

                dir_error_popup = true;
            }

            if (dir_error_popup)
            {
                ImGui::OpenPopup("Error");
                if (ImGui::BeginPopupModal("Error", &dir_error_popup))
                {
                    std::string error_message = "Error: can't open directory: "+save_dir_.filename().string();
                    ImGui::Text(error_message.c_str());
                    ImGui::EndPopup();
                }
                if (!dir_error_popup)
                {
                    // Close the save menu when closing the error popup: if we
                    // can not open the directory, we can not save anything.
                    save_menu_open_ = false;
                }
            }

            ImGui::Separator();

            // InputText for the file's name.
            ImGui::InputText("Filename", filename.data(), filename.size());
            std::string trimmed_filename = array_to_string(filename);
            trim(trimmed_filename);
            
            ImGui::Separator();

            // Save button (with a simple check for a empty filename)
            if (ImGui::Button("Save") && !trimmed_filename.empty())
            {
                // Open the output file.
                std::ofstream ofs (save_dir_/trimmed_filename);

                // Open the error popup if we can not open the file.
                if(!ofs.is_open())
                {
                    file_error_popup = true;
                }
                else
                {
                    // Save the LSystemView in the file.
                    cereal::JSONOutputArchive archive (ofs);
                    if (LSystemController::under_mouse()) // Virtually useless check.
                    {
                        archive(cereal::make_nvp("LSystemView", *LSystemController::under_mouse()));
                    }
                    save_menu_open_ = false;
                }
            }

            // File error popup if we can not open the output file.
            if (file_error_popup)
            {
                ImGui::OpenPopup("Error");
                if (ImGui::BeginPopupModal("Error", &file_error_popup))
                {
                    std::string message = "Error: can't open file: '" + array_to_string(filename) + "'";
                    ImGui::Text(message.c_str());
                    ImGui::EndPopup();
                }
            }

            // Fast close the save menu
            ImGui::SameLine();
            if (ImGui::Button("Cancel"))
            {
                save_menu_open_ = false;
            }
            
            ImGui::End();
        }
    }

    void WindowController::load_menu(std::list<procgui::LSystemView>& lsys_views)
    {
        // The file name in which will be save the LSystem.
        static std::array<char, FILENAME_LENGTH_> filename;
        // Flag to let the directory error popup open between frames.
        static bool dir_error_popup = false;
        // Flag to let the file error popup open between frames.
        static bool file_error_popup = false;

        ImGui::SetNextWindowPosCenter();
        if (ImGui::Begin("Load LSystem from file", &load_menu_open_, ImGuiWindowFlags_AlwaysAutoResize|ImGuiWindowFlags_NoCollapse|ImGuiWindowFlags_NoSavedSettings))
        {
            // Avoid interaction with the background when saving a file.
            ImGui::CaptureKeyboardFromApp();
            ImGui::CaptureMouseFromApp();

            ImGui::Separator();
            try
            {
                // For each file of the 'save_dir_' directory...
                for (const auto& file : fs::directory_iterator(save_dir_))
                {
                    // ... displays it in a selectable list ...
                    if (fs::is_regular_file(file.path()) &&
                        ImGui::Selectable(file.path().filename().c_str()))
                    {
                        // ... and set 'filename' to it when clicked (to
                        // load this file).
                        filename = string_to_array<filename.size()>(file.path().filename());
                    }
                }
            }
            catch (const fs::filesystem_error& e)
            {
                // If we can't open 'save_dir_', open an error popup.
                dir_error_popup = true;
            }

            if (dir_error_popup)
            {
                ImGui::OpenPopup("Error");
                if (ImGui::BeginPopupModal("Error", &dir_error_popup))
                {
                    std::string error_message = "Error: can't open directory: "+save_dir_.filename().string();
                    ImGui::Text(error_message.c_str());
                    ImGui::EndPopup();
                }
                if (!dir_error_popup)
                {
                    // Close the load menu when closing the error popup: if we
                    // can not open the directory, we can not load anything.
                    load_menu_open_ = false;
                }
            }

            ImGui::Separator();

            // Simple informative text
            std::string tmp = "File to load: '"+array_to_string(filename)+"'";
            ImGui::Text(tmp.c_str());
            ImGui::SameLine();

            if (ImGui::Button("Load"))
            {
                // Open the input file.
                std::ifstream ifs (save_dir_/array_to_string(filename));

                // Open the error popup if we can not open the file.
                if(!ifs.is_open())
                {
                    file_error_popup = true;
                }
                else
                {
                    // Create a default LSystemView.
                    procgui::LSystemView loaded_view({0,0});
                    try
                    {
                        // Load it from the file.
                        cereal::JSONInputArchive archive (ifs);
                        archive(loaded_view);
                    }
                    catch (const cereal::RapidJSONException& e)
                    {
                        // If the file is not in the correct format, open the
                        // error popup. 
                        file_error_popup = true;
                    }
                    if (!file_error_popup)
                    {
                        // Paste the new LSystemView at the correct position.
                        auto tmp = std::make_optional(loaded_view);
                        paste_view(lsys_views, tmp, mouse_position_to_load_);
                        load_menu_open_ = false;
                    }
                }
            }

            // File error popup if we can not open the file or if it is in the
            // wrong format.
            if (file_error_popup)
            {
                ImGui::OpenPopup("Error");
                if (ImGui::BeginPopupModal("Error", &file_error_popup))
                {
                    std::string message = "Error: can't open file: '" + array_to_string(filename) + "' (or wrong format)";
                    ImGui::Text(message.c_str());
                    ImGui::EndPopup();
                }
            }

            ImGui::SameLine();
            if (ImGui::Button("Cancel"))
            {
                load_menu_open_ = false;
            }
            
            ImGui::End();
        }
    }
    
    void WindowController::handle_input(std::vector<sf::Event> events,
                                        sf::RenderWindow &window,
                                        std::list<procgui::LSystemView>& lsys_views)
    {
        ImGuiIO& imgui_io = ImGui::GetIO();

        for(const auto& event : events)
        {
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
                    paste_view(lsys_views,
                               LSystemController::saved_view(),
                               real_mouse_position(sf::Mouse::getPosition(window)));
                }
                else if (event.key.code == sf::Keyboard::N)
                {
                    lsys_views.emplace_front(ext::sf::Vector2d(real_mouse_position(sf::Mouse::getPosition(window))));
                }
                else if (event.key.code == sf::Keyboard::O)
                {
                    mouse_position_to_load_ = real_mouse_position(sf::Mouse::getPosition(window));
                    load_menu_open_ = true;
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

        if (save_menu_open_)
        {
            save_menu();
        }
        if (load_menu_open_)
        {
            load_menu(lsys_views);
        }
        
        // The right-click menu depends on the location of the mouse.
        // We do not check if the mouse's right button was clicked, imgui takes
        // care of that.
        if (LSystemController::has_priority())
        {
            LSystemController::right_click_menu(lsys_views);
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
