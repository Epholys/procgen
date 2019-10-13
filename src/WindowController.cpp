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
        lsys_views.front().select();
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

    void WindowController::save_menu(sf::Keyboard::Key key)
    {
        // The file name in which will be save the LSystem.
        static std::array<char, FILENAME_LENGTH_> save_file;
        // Index of the file in the save menu
        static int selected_file = -1;
        // True if an existing save file is selected with the mouse.
        static bool click_selected = false;
        // Flag to open the directory error popup open between frames.
        static bool dir_error_popup = false;
        // Flag to open the file error popup open between frames.
        static bool file_error_popup = false;
        // Flag to open the save confirmation popup.
        static bool save_validation_popup = false;

        // Little helper to manage the use of 'Escape' key in popups.
        auto escape_popup_if_necessary = [](sf::Keyboard::Key& key, bool& popup)
            {
                if (key == sf::Keyboard::Escape)
                {
                    // Consume the key to avoid propagating it to another element.
                    key = sf::Keyboard::Unknown;
                    popup = false;
                    ImGui::CloseCurrentPopup();
                }
            };
        
        ImGui::SetNextWindowPosCenter();
        if (ImGui::Begin("Save LSystem to file", &save_menu_open_, ImGuiWindowFlags_NoCollapse|ImGuiWindowFlags_NoSavedSettings))
        {
            // Avoid interaction with the background when saving a file.
            ImGui::CaptureKeyboardFromApp();
            ImGui::CaptureMouseFromApp();

            ImGui::Separator();

            struct file_entry
            {
                fs::directory_entry file;
                std::string filename;
                std::u32string u32filename;
            };
                
            std::vector<file_entry> files;
            try
            {
                // Get all files in the 'save_dir_' directory, ...
                for (const auto& file : fs::directory_iterator(save_dir_))
                {
                    files.push_back({file,
                                file.path().filename().string(),
                                file.path().filename().u32string()});
                }

                // ... remove the directory, links, etc ...
                const auto to_remove = std::remove_if(begin(files),
                                                      end(files),
                                                      [](const auto& f){return !fs::is_regular_file(f.file.path());});
                files.erase(to_remove, end(files));
                                
                // ... sort them lexicographically, ...
                std::sort(begin(files), end(files),
                          [](const auto& left, const auto& right)
                          {
                              auto left_u32str = left.u32filename;
                              std::transform(begin(left_u32str), end(left_u32str), begin(left_u32str),
                                             [](auto c){return std::tolower(c);});
                              auto right_u32str = right.u32filename;
                              std::transform(begin(right_u32str), end(right_u32str), begin(right_u32str),
                                             [](auto c){return std::tolower(c);});
                              return left_u32str < right_u32str;                          
                          });
                
                // I'm bad at imgui's layout witchcraft, so there is a lot of
                // magic numbers here and there.
                constexpr float xfont_margin = -5.6;      // Little margin to adjust horizontal spacing of the font size
                constexpr float yfont_margin = 5;         // Little margin to adjust vertical spacing of the font size
                constexpr float separation_size = 10;     // Little margin to take care of the case : lots of small files
                constexpr float min_xsize = 300;         // Minimum horizontal size of the window, for the bottom text
                constexpr float ymargin = 100;            // Vertical margin for the text below
                const float hfont_size = ImGui::GetFontSize()+xfont_margin;    // Total horizontal font size
                const float vfont_size = ImGui::GetFontSize()+yfont_margin;    // Total vertical font size
                constexpr float xratio = 3/4.;           // Ratio of the horizontal size of the load window
                                                         //   in regards to the sfml window
                constexpr float yratio = 3/4.;           // Ratio of the vertical size of the load window
                                                         //   in regards to the sfml window
                const float max_xsize = sfml_window::window.getSize().x * xratio;  // Maximum x-size of the load window
                const float max_ysize = sfml_window::window.getSize().y * yratio;  // Maximum y-size of the load window

                const float total_vertical_size = vfont_size * files.size();    // Total vertical size of the file list
                const int n_column = (total_vertical_size / max_ysize)+1;       // Number of column deduced
                // Number of files per column, taking care that only the last column has less element
                int file_per_column = files.size() / n_column;
                file_per_column = file_per_column % n_column != 0 ? file_per_column + 1 : file_per_column;
                    
                float vertical_size = total_vertical_size / n_column;           // On-screen vertical size of the file list
                vertical_size = vertical_size == 0 ? 1 : vertical_size;         // '0' has a special value for imgui, put '1'

                // Iterator to the file with the biggest file name
                const auto longest_file = std::max_element(begin(files), end(files),
                                                           [](const auto& f1, const auto& f2)
                                                           {return f1.u32filename.size() <
                                                                   f2.u32filename.size();});

                int longest_file_size = 0;
                if (longest_file != end(files)) // Makes sure there are files.
                {
                    longest_file_size = longest_file->filename.size();                    
                }
                float total_horizontal_size =  (longest_file_size * hfont_size + separation_size) * n_column; // Total horizontal size of the file list, column included.
                total_horizontal_size = total_horizontal_size == 0 ? 1 : total_horizontal_size;               // '0' has a special value for imgui, put '1'
                float horizontal_size = total_horizontal_size < min_xsize ? min_xsize : total_horizontal_size;
                horizontal_size = horizontal_size < max_xsize ? horizontal_size : max_xsize;
                
                // The size of the load window.
                // x is the clamped total horizontal size
                // y is the vertical size of the list + space for the bottom text
                ImGui::SetWindowSize(ImVec2(horizontal_size, vertical_size + ymargin)); 

                // The virtual size of the files list. Virtual because it may not appear completely on screen, with scrollbar
                // x is the horizontal size of the columned files list
                // y is the vertical size of the list without the space for the bottom text
                ImGui::SetNextWindowContentSize(ImVec2(total_horizontal_size, vertical_size));

                //  Visible size of the files list
                //  x has the same size of the load window
                //  y has the same size of the content size
                ImGui::BeginChild("##ScrollingRegion", ImVec2(horizontal_size, vertical_size), false, ImGuiWindowFlags_HorizontalScrollbar);
                ImGui::Columns(n_column);
                
                for (auto i=0; i<(int)files.size(); ++i)
                {
                    const auto& file = files.at(i);

                    // Displays the files in a selectable list
                    if (ImGui::Selectable(file.filename.c_str(), selected_file == i))
                    {
                        // Update selected file to highlight the Selectable
                        selected_file = i;
                        // The user have selected with the mouse
                        click_selected = true;

                        // Updates the save file selection
                        save_file = string_to_array<save_file.size()>(file.filename);
                        
                    }
                    if ((i+1) % file_per_column == 0)
                    {
                        ImGui::NextColumn();
                    }
                }
                ImGui::Columns(1);
                ImGui::EndChild();

                // If there is no issue the popup should not be opened.
                // This is useful in the rare case when the file permissions for
                // the saves/ directory changes while the dir_error_popup is
                // open.
                dir_error_popup = false;

            }
            catch (const fs::filesystem_error& e)
            {
                // If we can't open 'save_dir_', open an error popup.

                dir_error_popup = true;
            }

            if (dir_error_popup)
            {
                ImGui::OpenPopup("Error##DIR");
                if (ImGui::BeginPopupModal("Error##DIR", &dir_error_popup))
                {
                    escape_popup_if_necessary(key, dir_error_popup);
                    
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

            // Allows directly typing a filename after opening the save menu.
            // Only focus if no popup is open
            if (!save_validation_popup && !dir_error_popup && !file_error_popup
                 && !ImGui::IsAnyItemActive())
            {
                ImGui::SetKeyboardFocusHere();
            }

            std::string trimmed_filename = trim(array_to_string(save_file));

            // If the user selected a save file:
            if (click_selected)
            {
                // InputText does not put automatically the cursor to the end
                // when selecting a file. As such, we use a InputText callback
                // to put the cursor to the end. The problem is, this callback
                // is executed each frame, but the first one does not count. So
                // we must execute this part for two frames, which is the role
                // of 'first_frame'.
                static bool first_frame = true;

                struct PutCursorEndCallback
                {
                    static int put_cursor_at_end(ImGuiInputTextCallbackData *data)
                        {
                            data->CursorPos = data->BufTextLen * sizeof(int);
                            return 0;
                        }
                };
                ImGui::InputText("Filename###SAME", save_file.data(), save_file.size(),
                                 ImGuiInputTextFlags_CallbackAlways,
                                 PutCursorEndCallback::put_cursor_at_end);

                if(!first_frame)
                {
                    click_selected = false;
                    first_frame = true;
                }
                else
                {
                    first_frame = false;
                }
            }
            else
            {
                if(ImGui::InputText("Filename###SAME", save_file.data(), save_file.size()))
                {
                    trimmed_filename = trim(array_to_string(save_file));

                    int i = -1;
                    auto same_name = std::find_if(begin(files), end(files),
                                                  [trimmed_filename, &i](const auto& f)
                                                  {++i; return trimmed_filename == f.filename;});
                    if (same_name != end(files))
                    {
                        selected_file = i;
                    }
                    else
                    {
                        selected_file = -1;
                    }
                }
            }

            ImGui::Separator();

            bool save = false;
            // Save button (with a simple check for a empty filename)
            if ((!save_validation_popup && !dir_error_popup && !file_error_popup) &&  // If no popup is open &&
                (ImGui::Button("Save") || key == sf::Keyboard::Enter) &&              // If the user want to save &&
                !trimmed_filename.empty())                                            // A valid filename
            {
                // Consume the key to avoid propagating it to another element.
                key = sf::Keyboard::Unknown;
                
                if (selected_file >= 0)
                {
                    save_validation_popup = true;
                }
                else
                {
                    save = true;
                }
            }
            
            if (save_validation_popup)
            {
                // Popup is now open, imgui takes care of the open/close state.
                ImGui::OpenPopup("Error##EXISTS");
                if (ImGui::BeginPopupModal("Error##EXISTS", &save_validation_popup,
                                           ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoResize))
                {
                    escape_popup_if_necessary(key, save_validation_popup);
                    
                    std::string warning_text = trimmed_filename + " already exists.\nDo you want to overwrite it?";
                    ImGui::Text(warning_text.c_str());

                    if (ImGui::Button("Overwrite") || key == sf::Keyboard::Enter)
                    {
                        key = sf::Keyboard::Unknown;
                        save = true;
                        save_validation_popup = false;
                    }

                    ImGui::SameLine();
                    if (ImGui::Button("Cancel"))
                    {
                        save_validation_popup = false;
                    }

                    ImGui::EndPopup();
                }
            }

            if (save)
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
                ImGui::OpenPopup("Error##PERM");
                if (ImGui::BeginPopupModal("Error##PERM", &file_error_popup))
                {
                    escape_popup_if_necessary(key, file_error_popup);
                    
                    std::string message = "Error: can't open file: '" + array_to_string(save_file) + "'";
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

        if (key == sf::Keyboard::Escape)
        {
            save_menu_open_ = false;
        }
    }

    void WindowController::add_loading_error_message(const std::string& message)
    {
        error_messages.push_back(message);
    }

    void WindowController::load_menu(std::list<procgui::LSystemView>& lsys_views,
                                     sf::Keyboard::Key key,
                                     sf::Uint32 unicode)
    {
        // The file name in which will be save the LSystem.
        static std::array<char, FILENAME_LENGTH_> file_to_load;
        static int selected_file = 0;
        // Flag to let the directory error popup open between frames.
        static bool dir_error_popup = false;
        // Flag to let the file error popup open between frames.
        static bool file_error_popup = false;

        static bool format_error_popup = false;

        static bool error_message_popup = false;
        // Flag to know if a file is selected twice (meaning two click or one
        // other selection and one click).
        // Used to quickly load a file with the mouse.
        static bool double_selection = false;

        // Little helper to manage the use of 'Escape' key in popups.
        auto escape_popup_if_necessary = [](sf::Keyboard::Key& key, bool& popup)
            {
                if (key == sf::Keyboard::Escape)
                {
                    // Consume the key to avoid propagating it to another element.
                    key = sf::Keyboard::Unknown;
                    popup = false;
                    ImGui::CloseCurrentPopup();
                }
            };
        
        ImGui::SetNextWindowPosCenter();
        if (ImGui::Begin("Load LSystem from file", &load_menu_open_, ImGuiWindowFlags_NoCollapse|ImGuiWindowFlags_NoSavedSettings))
        {
            // Avoid interaction with the background when saving a file.
            ImGui::CaptureKeyboardFromApp();
            ImGui::CaptureMouseFromApp();

            ImGui::Separator();

            // %%%%%%%%%%%%%%%% LOAD SCREEN %%%%%%%%%%%%%%%%
            try
            {
                struct file_entry
                {
                    fs::directory_entry file;
                    std::string filename;
                    std::u32string u32filename;
                };
                
                std::vector<file_entry> files;
                // Get all files in the 'save_dir_' directory, ...
                for (const auto& file : fs::directory_iterator(save_dir_))
                {
                    files.push_back({file,
                                file.path().filename().string(),
                                file.path().filename().u32string()});
                }

                // ... remove the directory, links, etc ...
                const auto to_remove = std::remove_if(begin(files),
                                                      end(files),
                                                      [](const auto& f){return !fs::is_regular_file(f.file.path());});
                files.erase(to_remove, end(files));
                                
                // ... sort them lexicographically, ...
                std::sort(begin(files), end(files),
                          [](const auto& left, const auto& right)
                          {
                              auto left_u32str = left.u32filename;
                              std::transform(begin(left_u32str), end(left_u32str), begin(left_u32str),
                                             [](auto c){return std::tolower(c);});
                              auto right_u32str = right.u32filename;
                              std::transform(begin(right_u32str), end(right_u32str), begin(right_u32str),
                                             [](auto c){return std::tolower(c);});
                              return left_u32str < right_u32str;                          
                          });
                
                // I'm bad at imgui's layout witchcraft, so there is a lot of
                // magic numbers here and there.
                constexpr float xfont_margin = -5.6;      // Little margin to adjust horizontal spacing of the font size
                constexpr float yfont_margin = 5;         // Little margin to adjust vertical spacing of the font size
                constexpr float separation_size = 10;     // Little margin to take care of the case : lots of small files
                float min_xsize = 250;         // Minimum horizontal size of the window, for the bottom text
                constexpr float ymargin = 70;            // Vertical margin for the text below
                const float hfont_size = ImGui::GetFontSize()+xfont_margin;    // Total horizontal font size
                const float vfont_size = ImGui::GetFontSize()+yfont_margin;    // Total vertical font size
                constexpr float xratio = 3/4.;           // Ratio of the horizontal size of the load window
                                                         //   in regards to the sfml window
                constexpr float yratio = 3/4.;           // Ratio of the vertical size of the load window
                                                         //   in regards to the sfml window
                const float max_xsize = sfml_window::window.getSize().x * xratio;  // Maximum x-size of the load window
                const float max_ysize = sfml_window::window.getSize().y * yratio;  // Maximum y-size of the load window

                const float total_vertical_size = vfont_size * files.size();    // Total vertical size of the file list
                const int n_column = (total_vertical_size / max_ysize)+1;       // Number of column deduced
                // Number of files per column, taking care that only the last column has less element
                int file_per_column = files.size() / n_column;
                file_per_column = file_per_column % n_column != 0 ? file_per_column + 1 : file_per_column;
                    
                float vertical_size = total_vertical_size / n_column;           // On-screen vertical size of the file list
                vertical_size = vertical_size == 0 ? 1 : vertical_size;         // '0' has a special value for imgui, put '1'

                // Iterator to the file with the biggest file name
                const auto longest_file = std::max_element(begin(files), end(files),
                                                           [](const auto& f1, const auto& f2)
                                                           {return f1.u32filename.size() <
                                                                   f2.u32filename.size();});

                int longest_file_size = 0;
                if (longest_file != end(files)) // Makes sure there are files.
                {
                    longest_file_size = longest_file->filename.size();
                }
                min_xsize += longest_file_size * hfont_size;
                float total_horizontal_size =  (longest_file_size * hfont_size + separation_size) * n_column; // Total horizontal size of the file list, column included.
                total_horizontal_size = total_horizontal_size == 0 ? 1 : total_horizontal_size;               // '0' has a special value for imgui, put '1'
                float horizontal_size = total_horizontal_size < min_xsize ? min_xsize : total_horizontal_size;
                horizontal_size = horizontal_size < max_xsize ? horizontal_size : max_xsize;
                
                // The size of the load window.
                // x is the clamped total horizontal size
                // y is the vertical size of the list + space for the bottom text
                ImGui::SetWindowSize(ImVec2(horizontal_size, vertical_size + ymargin)); 

                // The virtual size of the files list. Virtual because it may not appear completely on screen, with scrollbar
                // x is the horizontal size of the columned files list
                // y is the vertical size of the list without the space for the bottom text
                ImGui::SetNextWindowContentSize(ImVec2(total_horizontal_size, vertical_size));

                //  Visible size of the files list
                //  x has the same size of the load window
                //  y has the same size of the content size
                ImGui::BeginChild("##ScrollingRegion", ImVec2(horizontal_size, vertical_size), false, ImGuiWindowFlags_HorizontalScrollbar);
                ImGui::Columns(n_column);
                

                // Select the appropriate file if the user pressed a key
                if (unicode != 0)
                {
                    for (auto i=0u; i<files.size(); ++i)
                    {
                        const auto& f = files.at(i);
                        if (f.u32filename.size() > 0 &&
                            std::tolower(f.u32filename.at(0)) == std::tolower(unicode))
                        {
                            selected_file = i;
                            break;
                        }
                    }
                }
                if (!files.empty())
                {
                    if (key == sf::Keyboard::Key::Right)
                    {
                        selected_file += file_per_column; // Jump right a column
                        if (selected_file / file_per_column >= n_column) // If we are at the right edge
                        {
                            selected_file = selected_file % file_per_column; // Go to the beginning at the same height
                        }
                        else if (selected_file >= (int)files.size()) // We are below the last element in the last column
                        {
                            selected_file = files.size() - 1;
                        }
                    }
                    else if (key == sf::Keyboard::Key::Left)
                    {
                        const int jump_back = selected_file - file_per_column;
                        if (jump_back >= 0)
                        {
                            selected_file = jump_back;
                        }
                        else // We are at the left edge
                        {
                            // Go to the end at the same height
                            selected_file = ((n_column-1) * file_per_column) + selected_file;
                            if (selected_file >= (int)files.size()) // past the end
                            {
                                selected_file = files.size() - 1 ;
                            }
                        }
                    }
                    else if (key == sf::Keyboard::Key::Down)
                    {
                        ++selected_file;
                        if (selected_file > file_per_column * (n_column-1)) // File is in last column
                        {
                            if (selected_file == (int)files.size()) // If past last element
                            {
                                selected_file = file_per_column * (n_column - 1); // Go to the top of the last column
                            }
                        }
                        else // File not in last column
                        {
                            const int pos_in_column = selected_file % file_per_column;
                            if (pos_in_column % file_per_column == 0)
                            {
                                selected_file -= file_per_column;
                            }
                        }
                    }
                    else if (key == sf::Keyboard::Key::Up)
                    {
                        if (selected_file % file_per_column == 0) // Top of a column
                        {
                            if (selected_file >= file_per_column * (n_column-1)) // Last column
                            {
                                selected_file = files.size()-1;                  // Last element
                            }
                            else
                            {
                                selected_file += file_per_column-1;
                            }
                        }
                        else
                        {
                            --selected_file;
                        }
                    }
                }
                

                for (auto i=0; i<(int)files.size(); ++i)
                {
                    const auto& file = files.at(i);

                    bool selectable_clicked = ImGui::Selectable(file.filename.c_str(), selected_file == i);
                    if (selectable_clicked && selected_file == i && !double_selection)
                    {
                        // The selectable was previously selected
                            double_selection = true;
                    }
                    // Displays the files in a selectable list
                    if (selectable_clicked || selected_file == i)
                    {
                        // If the file was selected by clicking on the Selectable, update the selected_file
                        selected_file = i;
                        
                        file_to_load = string_to_array<file_to_load.size()>(file.filename);
                    }
                    if ((i+1) % file_per_column == 0)
                    {
                        ImGui::NextColumn();
                    }
                }
                ImGui::Columns(1);
                ImGui::EndChild();

                // If there is no issue the popup should not be opened.
                // This is useful in the rare case when the file permissions for
                // the saves/ directory changes while the dir_error_popup is
                // open.
                dir_error_popup = false;
            }
            catch (const fs::filesystem_error& e)
            {
                // If we can't open 'save_dir_', open an error popup.
                dir_error_popup = true;
            }

            if (dir_error_popup)
            {
                ImGui::OpenPopup("Error##DIR");
                if (ImGui::BeginPopupModal("Error##DIR", &dir_error_popup))
                {
                    escape_popup_if_necessary(key, dir_error_popup);

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
            std::string tmp = "File to load: '"+array_to_string(file_to_load)+"'";
            ImGui::Text(tmp.c_str());
            ImGui::SameLine();

            // %%%%%%%%%%%%%%%% LOAD LSYSTEMVIEW %%%%%%%%%%%%%%%%
            
            if ((!dir_error_popup && !error_message_popup && !file_error_popup && !format_error_popup) && // If no popup is open &&
                (ImGui::Button("Load") || key == sf::Keyboard::Enter || double_selection) &&              // If the user want to load &&
                !array_to_string(file_to_load).empty())                                                   // an existing file
            {
                double_selection = false;
                
                // Open the input file.
                std::ifstream ifs (save_dir_/array_to_string(file_to_load));

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
                ImGui::OpenPopup("Error##FILE");
                if (ImGui::BeginPopupModal("Error##FILE", &file_error_popup))
                {
                    escape_popup_if_necessary(key, file_error_popup);
                    
                    std::string message = "Error: can't open file: '" + array_to_string(file_to_load) + "'";
                    ImGui::Text(message.c_str());
                    ImGui::EndPopup();
                }
            }

            if (format_error_popup)
            {
                ImGui::OpenPopup("Error##FORMAT");
                if (ImGui::BeginPopupModal("Error##FORMAT", &format_error_popup))
                {
                    escape_popup_if_necessary(key, format_error_popup);
                    
                    std::string message = "Error: file '" + array_to_string(file_to_load) + "' isn't a valid or complete JSON L-System file.";
                    ImGui::Text(message.c_str());
                    ImGui::EndPopup();
                }
            }

            if (error_message_popup)
            {
                ImGui::OpenPopup("Warning##ISSUE");
                if (ImGui::BeginPopupModal("Warning##ISSUE", &error_message_popup))
                {
                    escape_popup_if_necessary(key, error_message_popup);
                    
                    std::string message;
                    if (error_messages.size() > 1)
                    {
                        message = "Warning: file '" + array_to_string(file_to_load) + "' has some issues:\n";
                    }
                    else
                    {
                        message = "Warning: file '" + array_to_string(file_to_load) + "' has one issue:\n";
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

        if (key == sf::Keyboard::Escape)
        {
            load_menu_open_ = false;
        }
    }
    
    void WindowController::handle_input(std::vector<sf::Event> events,
                                        std::list<procgui::LSystemView>& lsys_views)
    {
        ImGuiIO& imgui_io = ImGui::GetIO();
        
        sf::Keyboard::Key key_to_menus = sf::Keyboard::Unknown;
        sf::Uint32 unicode_to_load_window = 0;
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

            // Paste, Create, Load LSystemView 
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
                     event.type == sf::Event::KeyPressed)
            {
                key_to_menus = event.key.code;
            }
            else if (load_menu_open_ &&
                     event.type == sf::Event::TextEntered)
            {
                unicode_to_load_window = event.text.unicode;
            }

            // Save menu forwarding
            else if (save_menu_open_ &&
                     event.type == sf::Event::KeyPressed)
            {
                key_to_menus = event.key.code;
            }

            // SFML Window management
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
            save_menu(key_to_menus);
        }
        if (load_menu_open_)
        {
            load_menu(lsys_views, key_to_menus, unicode_to_load_window);
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
