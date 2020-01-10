#include "SaveMenu.h"

#include "LSystemController.h"
#include "LSystemView.h"
#include "PopupGUI.h"
#include "RenderWindow.h"
#include "cereal/archives/json.hpp"
#include "helper_string.h"
#include "imgui/imgui.h"
#include "imgui_extension.h"

#include <fstream>

namespace controller
{
SaveMenu::~SaveMenu()
{
    for (auto id : popups_ids_)
    {
        procgui::remove_popup(id);
    }
}

void SaveMenu::save_lsys(const std::string& trimmed_filename)
{
    // Open the output file.
    std::ofstream ofs(save_dir_ / trimmed_filename);

    // Open the error popup if we can not open the file.
    if (!ofs.is_open())
    {
        // Open the file error popup if the save file is inaccessible
        procgui::PopupGUI file_error_popup = {
            "Error##PERM",
            [trimmed_filename]() {
                std::string error_message = "Error: can't open file: " + trimmed_filename;
                ImGui::Text("%s", error_message.c_str());
            }};

        popups_ids_.push_back(procgui::push_popup(file_error_popup));
    }
    else
    {
        // Save the LSystemView in the file.
        cereal::JSONOutputArchive archive(ofs);
        if (LSystemController::under_mouse() != nullptr) // Virtually useless check,
                                                         // but here to avoid null
                                                         // pointer crash at worst
        {
            LSystemController::under_mouse()->set_name(trimmed_filename);
            archive(cereal::make_nvp("LSystemView", *LSystemController::under_mouse()));
        }

        close_menu_ = true;
    }
}

bool SaveMenu::save_button(const std::string& trimmed_filename, sf::Keyboard::Key& key)
{
    bool save = false;

    ext::ImGui::PushStyleColoredButton<ext::ImGui::Green>();
    if (procgui::popup_empty() && // If no popup is open &&
        (ImGui::Button("Save") || key == sf::Keyboard::Enter || double_selection_)
        &&                         // If the user want to save &&
        !trimmed_filename.empty()) // A valid filename
    {
        double_selection_ = false;

        // Consume the key to avoid propagating it to another element.
        key = sf::Keyboard::Unknown;

        if (selected_file_ >= 0)
        {
            // If the file already exsists, open a validation popup
            procgui::PopupGUI save_validation_popup = {
                "Error##EXISTS",
                [trimmed_filename]() {
                    std::string warning_text = trimmed_filename
                                               + " already exists.\nDo you want to overwrite it?";
                    ImGui::Text("%s", warning_text.c_str());
                },
                false,
                "Overwrite",
                "Cancel",
                [this, trimmed_filename]() { save_lsys(trimmed_filename); }};

            popups_ids_.push_back(procgui::push_popup(save_validation_popup));
        }
        else
        {
            save = true;
        }
    }
    ImGui::PopStyleColor(3);

    return save;
}

void SaveMenu::input_text_field(const std::vector<file_entry>& save_files,
                                std::string& trimmed_filename)
{
    // Allows directly typing a filenxame after opening the save menu.
    // Only focus if no popup is open
    if (procgui::popup_empty() && !ImGui::IsAnyItemActive())
    {
        ImGui::SetKeyboardFocusHere();
    }

    // If the user selected a save file:
    if (first_menu_frame_ || click_selected_ || !first_input_frame_)
    {
        // InputText does not put automatically the cursor to the end
        // when selecting a file. As such, we use a InputText callback
        // to put the cursor to the end. The problem is, this callback
        // is executed each frame, but the first one does not count. So
        // we must execute this part for two frames, which is the role
        // of 'first_frame'.

        struct PutCursorEndCallback
        {
            static int put_cursor_at_end(ImGuiInputTextCallbackData* data)
            {
                data->CursorPos = data->BufTextLen * sizeof(int);
                return 0;
            }
        };
        ImGui::InputText("Filename###SAME",
                         filename_.data(),
                         filename_.size(),
                         ImGuiInputTextFlags_CallbackAlways,
                         PutCursorEndCallback::put_cursor_at_end);

        if (!first_input_frame_)
        {
            click_selected_ = false;
            first_input_frame_ = true;
        }
        else
        {
            first_input_frame_ = false;
        }
    }
    else
    {
        if (ImGui::InputText("Filename###SAME", filename_.data(), filename_.size()))
        {
            trimmed_filename = trim(array_to_string(filename_));

            int i = -1;
            auto same_name = std::find_if(begin(save_files),
                                          end(save_files),
                                          [trimmed_filename, &i](const auto& f) {
                                              ++i;
                                              return trimmed_filename == f.filename;
                                          });
            if (same_name != end(save_files))
            {
                selected_file_ = i;
            }
            else
            {
                selected_file_ = -1;
            }
        }
    }
}


SaveMenu::column_layout SaveMenu::list_layout(const std::vector<file_entry>& save_files)
{
    // I'm bad at imgui's layout witchcraft, so there is a lot of
    // magic numbers here and there.
    constexpr float xfont_margin =
        -5.6;                         // Little margin to adjust horizontal spacing of the font size
    constexpr float yfont_margin = 5; // Little margin to adjust vertical spacing of the font size
    constexpr float separation_size =
        10;                          // Little margin to take care of the case : lots of small files
    constexpr float min_xsize = 300; // Minimum horizontal size of the window, for the bottom text
    constexpr float ymargin = 100;   // Vertical margin for the text below
    const float hfont_size = ImGui::GetFontSize() + xfont_margin; // Total horizontal font size
    const float vfont_size = ImGui::GetFontSize() + yfont_margin; // Total vertical font size
    constexpr float xratio = 3 / 4.; // Ratio of the horizontal size of the load window
    //   in regards to the sfml window
    constexpr float yratio = 3 / 4.; // Ratio of the vertical size of the load window
    //   in regards to the sfml window
    const float max_xsize = sfml_window::window.getSize().x
                            * xratio; // Maximum x-size of the load window
    const float max_ysize = sfml_window::window.getSize().y
                            * yratio; // Maximum y-size of the load window

    const float total_vertical_size = vfont_size
                                      * save_files.size(); // Total vertical size of the file list
    const int n_column = (total_vertical_size / max_ysize) + 1; // Number of column deduced
    // Number of files per column, taking care that only the last column has less element
    int file_per_column = save_files.size() / n_column;
    file_per_column = file_per_column % n_column != 0 ? file_per_column + 1 : file_per_column;

    float vertical_size = total_vertical_size
                          / n_column; // On-screen vertical size of the file list
    vertical_size = vertical_size == 0
                        ? 1
                        : vertical_size; // '0' has a special value for imgui, put '1'

    // Iterator to the file with the biggest file name
    const auto longest_file =
        std::max_element(begin(save_files), end(save_files), [](const auto& f1, const auto& f2) {
            return f1.u32filename.size() < f2.u32filename.size();
        });

    int longest_file_size = 0;
    if (longest_file != end(save_files)) // Makes sure there are files.
    {
        longest_file_size = longest_file->filename.size();
    }
    float total_horizontal_size =
        (longest_file_size * hfont_size + separation_size)
        * n_column; // Total horizontal size of the file list, column included.
    total_horizontal_size =
        total_horizontal_size == 0
            ? 1
            : total_horizontal_size; // '0' has a special value for imgui, put '1'
    float horizontal_size = total_horizontal_size < min_xsize ? min_xsize : total_horizontal_size;
    horizontal_size = horizontal_size < max_xsize ? horizontal_size : max_xsize;

    // The size of the load window.
    // x is the clamped total horizontal size
    // y is the vertical size of the list + space for the bottom text
    ImGui::SetWindowSize(ImVec2(horizontal_size, vertical_size + ymargin));

    // The virtual size of the files list. Virtual because it may not appear completely on screen,
    // with scrollbar x is the horizontal size of the columned files list y is the vertical size of
    // the list without the space for the bottom text
    ImGui::SetNextWindowContentSize(ImVec2(total_horizontal_size, vertical_size));

    //  Visible size of the files list
    //  x has the same size of the load window
    //  y has the same size of the content size
    ImGui::BeginChild("##ScrollingRegion",
                      ImVec2(horizontal_size, vertical_size),
                      false,
                      ImGuiWindowFlags_HorizontalScrollbar);

    return {n_column, file_per_column};
}


void SaveMenu::list(std::vector<file_entry>& save_files)
{
    try
    {
        // Get all files in the 'save_dir_' directory, ...
        for (const auto& file : fs::directory_iterator(save_dir_))
        {
            save_files.push_back(
                {file, file.path().filename().string(), file.path().filename().u32string()});
        }

        // ... remove the directory, links, etc ...
        const auto to_remove = std::remove_if(
            begin(save_files),
            end(save_files),
            [](const auto& f) { return !fs::is_regular_file(f.file.path()); });
        save_files.erase(to_remove, end(save_files));

        // ... sort them lexicographically, ...
        std::sort(begin(save_files), end(save_files), [](const auto& left, const auto& right) {
            auto left_u32str = left.u32filename;
            std::transform(begin(left_u32str), end(left_u32str), begin(left_u32str), [](auto c) {
                return std::tolower(c);
            });
            auto right_u32str = right.u32filename;
            std::transform(begin(right_u32str), end(right_u32str), begin(right_u32str), [](auto c) {
                return std::tolower(c);
            });
            return left_u32str < right_u32str;
        });


        auto [n_column, file_per_column] = list_layout(save_files);

        ImGui::Columns(n_column);

        for (auto i = 0; i < (int)save_files.size(); ++i)
        {
            const auto& file = save_files.at(i);

            if (array_to_string(filename_) == file.filename)
            {
                // Update selected file to highlight the Selectable
                selected_file_ = i;
            }

            // Displays the files in a selectable list
            if (ImGui::Selectable(file.filename.c_str(), selected_file_ == i))
            {
                if (!double_selection_ && selected_file_ == i)
                {
                    double_selection_ = true;
                }

                // The user have selected with the mouse
                click_selected_ = true;

                // Updates the save file selection
                filename_ = string_to_array<FILENAME_LENGTH_>(file.filename);
            }
            if ((i + 1) % file_per_column == 0)
            {
                ImGui::NextColumn();
            }
        }
        ImGui::Columns(1);
        ImGui::EndChild();
    }
    catch (const fs::filesystem_error& e)
    {
        // Open a popup if the directory can't be opened
        procgui::PopupGUI dir_error_popup = {
            "Error##DIR",
            [this]() {
                std::string error_message = "Error: can't open directory: "
                                            + save_dir_.filename().string();
                ImGui::Text("%s", error_message.c_str());
            }};
        popups_ids_.push_back(procgui::push_popup(dir_error_popup));
        close_menu_ = true;
    }
}

bool SaveMenu::open(sf::Keyboard::Key key, const std::string& filename)
{
    ImGui::SetNextWindowPosCenter();
    if (ImGui::Begin("Save LSystem to file",
                     nullptr,
                     ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoSavedSettings))
    {
        if (first_menu_frame_)
        {
            filename_ = string_to_array<FILENAME_LENGTH_>(filename);
        }

        std::vector<file_entry> save_files;

        // Avoid interaction with the background when saving a file.
        ImGui::CaptureKeyboardFromApp();
        ImGui::CaptureMouseFromApp();

        ImGui::Separator();

        list(save_files);

        ImGui::Separator();

        std::string trimmed_filename = trim(array_to_string(filename_));

        input_text_field(save_files, trimmed_filename);

        ImGui::Separator();

        bool save = save_button(trimmed_filename, key);

        if (save)
        {
            save_lsys(trimmed_filename);
        }

        ImGui::SameLine();

        ext::ImGui::PushStyleColoredButton<ext::ImGui::Red>();
        if (ImGui::Button("Cancel") || key == sf::Keyboard::Escape)
        {
            close_menu_ = true;
        }
        ImGui::PopStyleColor(3);

        ImGui::End();

        first_menu_frame_ = false;
    }

    if (close_menu_)
    {
        // Reset flags
        click_selected_ = false;
        double_selection_ = false;
        close_menu_ = false;
        first_menu_frame_ = true;
        return true;
    }

    return false;
}
} // namespace controller
