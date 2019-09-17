#include <fstream>

#include "imgui/imgui.h"
#include "cereal/archives/json.hpp"

#include "RenderWindow.h"
#include "helper_string.h"
#include "WindowController.h"
#include "LSystemController.h"
#include "LSystemView.h"

using sfml_window::window;
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

    const double WindowController::default_step_ {25.f}; 
    
    fs::path WindowController::save_dir_ = fs::u8path(u8"saves");

    std::vector<std::string> WindowController::error_messages {};
    
    sf::Vector2f WindowController::real_mouse_position(sf::Vector2i mouse_click)
    {
        return window.mapPixelToCoords(mouse_click);
    }

    sf::Vector2i WindowController::absolute_mouse_position(sf::Vector2f mouse_click)
    {
        return window.mapCoordsToPixel(mouse_click);
    }

    
    sf::Vector2i WindowController::get_mouse_position()
    {
        return mouse_position_;
    }

    float WindowController::get_zoom_level()
    {
        return zoom_level_;
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
        auto pasted_view = *view;
        auto box = pasted_view.get_bounding_box();
        ext::sf::Vector2d pos {position};
        ext::sf::Vector2d middle = {box.left + box.width/2, box.top + box.height/2};
        middle = pasted_view.get_parameters().get_starting_position() - middle;
        pasted_view.ref_parameters().set_starting_position(pos + middle);
        lsys_views.emplace_front(pasted_view);
    }
    
    void WindowController::right_click_menu(sf::RenderWindow& window, std::list<procgui::LSystemView>& lsys_views)
    {
        if (ImGui::BeginPopupContextVoid())
        {
            if (ImGui::MenuItem("New LSystem", "Ctrl+N"))
            {
                lsys_views.emplace_front(procgui::LSystemView(ext::sf::Vector2d(real_mouse_position(sf::Mouse::getPosition(window))),
                                                              default_step_ * zoom_level_));
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
                std::vector<fs::directory_entry> files;
                // Get all files in the 'save_dir_', ...
                for (const auto& file : fs::directory_iterator(save_dir_))
                {
                    files.emplace_back(file);
                }

                // ... sort them lexicographically, ...
                std::sort(begin(files), end(files),
                          [](const auto& left, const auto& right)
                          {
                              auto left_str = left.path().string();
                              std::transform(begin(left_str), end(left_str), begin(left_str),
                                             [](unsigned char c){return std::tolower(c);});
                              auto right_str = right.path().string();
                              std::transform(begin(right_str), end(right_str), begin(right_str),
                                             [](unsigned char c){return std::tolower(c);});
                              return left_str < right_str;                          
                          });

                for (const auto& file : files)
                {
                   // ... display them in a selectable list ...
                   if (fs::is_regular_file(file.path()) &&
                        ImGui::Selectable(file.path().filename().c_str()))
                    {
                        // ... and set 'filename' to if one is clicked (to overwrite this file).
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

    void WindowController::add_loading_error_message(const std::string& message)
    {
        error_messages.push_back(message);
    }

    void WindowController::load_menu(std::list<procgui::LSystemView>& lsys_views, sf::Keyboard::Key key)
    {
        // The file name in which will be save the LSystem.
        static std::array<char, FILENAME_LENGTH_> filename;
        // Flag to let the directory error popup open between frames.
        static bool dir_error_popup = false;
        // Flag to let the file error popup open between frames.
        static bool file_error_popup = false;

        static bool format_error_popup = false;

        static bool error_message_popup = false;

        ImGui::SetNextWindowPosCenter();
        if (ImGui::Begin("Load LSystem from file", &load_menu_open_, ImGuiWindowFlags_NoCollapse|ImGuiWindowFlags_NoSavedSettings))
        {
            // Avoid interaction with the background when saving a file.
            ImGui::CaptureKeyboardFromApp();
            ImGui::CaptureMouseFromApp();

            ImGui::Separator();
            try
            {
                std::vector<fs::directory_entry> files;
                // Get all files in the 'save_dir_' directory, ...
                for (const auto& file : fs::directory_iterator(save_dir_))
                {
                    files.emplace_back(file);
                }
                // ... sort them lexicographically, ...
                std::sort(begin(files), end(files),
                          [](const auto& left, const auto& right)
                          {
                              auto left_str = left.path().string();
                              std::transform(begin(left_str), end(left_str), begin(left_str),
                                             [](unsigned char c){return std::tolower(c);});
                              auto right_str = right.path().string();
                              std::transform(begin(right_str), end(right_str), begin(right_str),
                                             [](unsigned char c){return std::tolower(c);});
                              return left_str < right_str;                          
                          });

                // ... remove the directory, links, etc ...
                std::remove_if(begin(files), end(files), [](const auto& f){return !fs::is_regular_file(f.path());});

                // I'm bad at imgui's layout witchcrasft, so there is a lot of
                // magic numbers here and there.
                constexpr float font_margin = 10;                                // Little margin to add spacing
                const float font_size = ImGui::GetFontSize()+font_margin;        // Font and spacing
                const float max_x_size = sfml_window::window.getSize().x * 2/3;  // Maximum x-size of the load window
                const float max_y_size = sfml_window::window.getSize().y * 2/3;  // Maximum y-size of the load window

                const float total_vertical_size = font_size * files.size();      // Vertical size of the file list
                const int n_column = (total_vertical_size / max_y_size)+1;       // Number of column deduced
                const int file_per_column = files.size() / n_column;             // Explicit
                const float vertical_size = total_vertical_size / n_column;      // Useful if the list is small
                    
                const auto longest_file = std::max_element(begin(files), end(files));  // Iterator to the file with the biggest file name
                const int longest_file_size = longest_file->path().filename().string().size();
                const float total_horizontal_size =  longest_file_size * font_size * n_column;
                const float horizontal_size = total_horizontal_size < max_x_size ? total_horizontal_size : max_x_size;

                const float x_margin = 80, y_margin = 70; // Margins for the text below
                ImGui::SetWindowSize(ImVec2(horizontal_size + x_margin, vertical_size + y_margin));

                ImGui::SetNextWindowContentSize(ImVec2(total_horizontal_size, 0.0f)); // Total size of the scrolling area
                //  Max size taken by the file list
                ImGui::BeginChild("##ScrollingRegion", ImVec2(horizontal_size, vertical_size), false, ImGuiWindowFlags_HorizontalScrollbar);
                ImGui::Columns(n_column);
                
                for (auto i=1u; i<=files.size(); ++i)
                {
                    const auto& file = files.at(i-1);
                    // ... displays them in a selectable list ...
                    if (ImGui::Selectable(file.path().filename().c_str()))
                    {
                        // ... and set 'filename' to the one clicked (to
                        // load this file).
                        filename = string_to_array<filename.size()>(file.path().filename());
                    }
                    if (i % file_per_column == 0)
                    {
                        ImGui::NextColumn();
                    }
                }
                ImGui::Columns(1);
                ImGui::EndChild();
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

            if (!array_to_string(filename).empty() &&
                (ImGui::Button("Load") ||
                 key == sf::Keyboard::Enter))
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
                    procgui::LSystemView loaded_view({0,0}, default_step_ * zoom_level_);
                    try
                    {
                        // Load it from the file.
                        cereal::JSONInputArchive archive (ifs);
                        archive(loaded_view);
                    }
                    catch (const cereal::RapidJSONException& e)
                    {
                        // If the file is not in the correct format, open the
                        // format error popup. 
                        format_error_popup = true;
                    }
                    if (!file_error_popup && !format_error_popup)
                    {
                        // Redimension the L-System to take 2/3 of the lowest
                        // screen. Double the load time, but it should be okay
                        // except for huge L-Systems.
                        const double target_ratio = 2. / 3.;
                        double step {0};
                        auto box = loaded_view.get_bounding_box();
                        auto window_size = sfml_window::window.getSize();
                        float xratio = window_size.x / box.width;
                        float yratio = window_size.y / box.height;
                        if(xratio < yratio)
                        {

                            double target_size = target_ratio * window_size.x;
                            double diff_ratio = box.width != 0 ? target_size / box.width : target_size;
                            step = loaded_view.get_parameters().get_step() * diff_ratio * zoom_level_;
                        }
                        else
                        {
                            double target_size = target_ratio * window_size.y;
                            double diff_ratio = box.height != 0 ? target_size / box.height : target_size;
                            step = loaded_view.get_parameters().get_step() * diff_ratio * zoom_level_;
                        }
                        loaded_view.ref_parameters().set_step(step);
                                                
                        // Paste the new LSystemView at the correct position.
                        auto tmp = std::make_optional(loaded_view);
                        paste_view(lsys_views, tmp, mouse_position_to_load_);

                        load_menu_open_ = false;
                    }

                    if (!error_messages.empty())
                    {
                        error_message_popup = true;
                        load_menu_open_ = true;
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
                    std::string message = "Error: can't open file: '" + array_to_string(filename) + "'";
                    ImGui::Text(message.c_str());
                    ImGui::EndPopup();
                }
            }

            if (format_error_popup)
            {
                ImGui::OpenPopup("Error");
                if (ImGui::BeginPopupModal("Error", &format_error_popup))
                {
                    std::string message = "Error: file '" + array_to_string(filename) + "' isn't a valid or complete JSON L-System file.";
                    ImGui::Text(message.c_str());
                    ImGui::EndPopup();
                }
            }

            if (error_message_popup)
            {
                ImGui::OpenPopup("Warning");
                if (ImGui::BeginPopupModal("Warning", &error_message_popup))
                {
                    std::string message;
                    if (error_messages.size() > 1)
                    {
                        message = "Warning: file '" + array_to_string(filename) + "' has some issues:\n";
                    }
                    else
                    {
                        message = "Warning: file '" + array_to_string(filename) + "' has one issue:\n";
                    }
                    ImGui::Text(message.c_str());

                    for (const auto& error_message : error_messages)
                    {
                        std::string message = "\t- "+ error_message + "\n";
                        ImGui::Text(message.c_str());
                    }

                    if (error_messages.size() > 1)
                    {
                        ImGui::Text("These issues have been automatically corrected.\n");
                        ImGui::Text("Don't forget to save this L-System if you want to save these corrections.");
                    }
                    else
                    {
                        ImGui::Text("This issue has been automatically corrected.\n");
                        ImGui::Text("Don't forget to save this L-System if you want to save this correction.");
                    }
                    
                    ImGui::EndPopup();
                }
                else
                {
                    error_message_popup = false;
                    error_messages.clear();
                    load_menu_open_ = false;
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
                                        std::list<procgui::LSystemView>& lsys_views)
    {
        ImGuiIO& imgui_io = ImGui::GetIO();
        
        sf::Keyboard::Key key_to_load_window = sf::Keyboard::KeyCount;
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
                    lsys_views.emplace_front(procgui::LSystemView(ext::sf::Vector2d(real_mouse_position(sf::Mouse::getPosition(window))),
                                                                  default_step_ * zoom_level_));
                }
                else if (event.key.code == sf::Keyboard::O)
                {
                    mouse_position_to_load_ = real_mouse_position({int(sfml_window::window.getSize().x/2),
                                                                   int(sfml_window::window.getSize().y/2)});
                    load_menu_open_ = true;
                }
            }

            else if (load_menu_open_ &&
                     event.type == sf::Event::KeyPressed &&
                     event.key.code == sf::Keyboard::Enter)
            {
                key_to_load_window = sf::Keyboard::Enter;
            }

            else if (load_menu_open_ &&
                     event.type == sf::Event::KeyPressed &&
                     event.key.code == sf::Keyboard::Escape)
            {
                load_menu_open_ = false;
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
                view_.setSize(event.size.width*zoom_level_, event.size.height*zoom_level_);
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

                else if (event.type == sf::Event::MouseButtonReleased &&
                         event.mouseButton.button == sf::Mouse::Left)
                {
                    view_can_move_ = false;
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
            load_menu(lsys_views, key_to_load_window);
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
