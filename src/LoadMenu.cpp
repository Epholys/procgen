#include "LoadMenu.h"

#include "LSystemController.h"
#include "LSystemView.h"
#include "PopupGUI.h"
#include "RenderWindow.h"
#include "WindowController.h"
#include "cereal/archives/json.hpp"
#include "helper_string.h"
#include "imgui/imgui.h"
#include "imgui_extension.h"

#include <cctype>
#include <fstream>

namespace controller
{
// Global error message access in deserialization of LSys
std::vector<std::string> LoadMenu::error_messages;

LoadMenu::~LoadMenu()
{
    for (auto id : popups_ids_)
    {
        procgui::remove_popup(id);
    }
}


void LoadMenu::add_loading_error_message(const std::string& message)
{
    error_messages.push_back(message);
}

void LoadMenu::load(std::list<procgui::LSystemView>& lsys_views,
                    ext::sf::Vector2d load_position,
                    std::ifstream& ifs)
{
    bool no_error = true;

    // Create a default LSystemView.
    procgui::LSystemView loaded_view({0, 0}, WindowController::default_step_);
    try
    {
        // Load it from the file.
        cereal::JSONInputArchive archive(ifs);
        archive(loaded_view);
    }
    catch (const cereal::RapidJSONException& e)
    {
        // Open a popup if the save file is not in valid JSON format.

        procgui::PopupGUI format_popup = {
            "Error##FORMAT",
            [this]() {
                std::string message = "Error: file '" + array_to_string(file_to_load_)
                                      + "' isn't a valid or complete JSON L-System file.";
                ImGui::Text("%s", message.c_str());
            }};

        popups_ids_.push_back(procgui::push_popup(format_popup));

        no_error = false;
    }
    if (no_error)
    {
        lsys_views.emplace_front(std::move(loaded_view));
        lsys_views.front().ref_parameters().set_starting_position(load_position);
        lsys_views.front().finish_loading();
        lsys_views.front().select();

        close_menu_ = true;
    }


    if (!error_messages.empty())
    {
        // Open a final warning popup if there were error in the save file.

        procgui::PopupGUI warning_popup = {
            "Warning##ISSUE",
            [this]() {
                std::string message;
                if (error_messages.size() > 1)
                {
                    message = "Warning: file '" + array_to_string(file_to_load_)
                              + "' has some issues:\n";
                }
                else
                {
                    message = "Warning: file '" + array_to_string(file_to_load_)
                              + "' has one issue:\n";
                }
                ImGui::Text("%s", message.c_str());

                for (const auto& error_message : error_messages)
                {
                    std::string message = "\t- " + error_message + "\n";
                    ImGui::Text("%s", message.c_str());
                }

                if (error_messages.size() > 1)
                {
                    ImGui::Text("These issues have been automatically corrected.\n");
                    ImGui::Text("Don't forget to save this L-System if you want to save these "
                                "corrections.");
                }
                else
                {
                    ImGui::Text("This issue has been automatically corrected.\n");
                    ImGui::Text(
                        "Don't forget to save this L-System if you want to save this correction.");
                }
            }};

        popups_ids_.push_back(procgui::push_popup(warning_popup));
    }
}

void LoadMenu::load_button(std::list<procgui::LSystemView>& lsys_views,
                           ext::sf::Vector2d load_position,
                           sf::Keyboard::Key& key)
{
    ext::ImGui::PushStyleColoredButton<ext::ImGui::Green>();
    if (procgui::popup_empty() && // If no popups are open &&
        (ImGui::Button("Load") || key == sf::Keyboard::Enter || double_selection_)
        &&                                       // If the user want to load &&
        !array_to_string(file_to_load_).empty()) // an existing file
    {
        double_selection_ = false;

        // Open the input file.
        std::ifstream ifs(save_dir_ / array_to_string(file_to_load_));

        // Open the error popup if we can not open the file.
        if (!ifs.is_open())
        {
            procgui::PopupGUI file_error_popup = {
                "Error##PERM",
                [this]() {
                    // TODO true name
                    std::string error_message = "Error: can't open file: "
                                                + array_to_string(file_to_load_);
                    ImGui::Text("%s", error_message.c_str());
                }};

            popups_ids_.push_back(procgui::push_popup(file_error_popup));
        }
        else
        {
            load(lsys_views, load_position, ifs);
        }
    }
    ImGui::PopStyleColor(3);
}

LoadMenu::column_layout LoadMenu::list_layout(const std::vector<file_entry>& files)
{
    // I'm bad at imgui's layout witchcraft, so there is a lot of
    // magic numbers here and there.
    constexpr float xfont_margin =
        -5.6;                         // Little margin to adjust horizontal spacing of the font size
    constexpr float yfont_margin = 5; // Little margin to adjust vertical spacing of the font size
    constexpr float separation_size =
        10;                       // Little margin to take care of the case : lots of small files
    float min_xsize = 250;        // Minimum horizontal size of the window, for the bottom text
    constexpr float ymargin = 70; // Vertical margin for the text below
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
                                      * files.size(); // Total vertical size of the file list
    const int n_column = (total_vertical_size / max_ysize) + 1; // Number of column deduced
    // Number of files per column, taking care that only the last column has less element
    int file_per_column = files.size() / n_column;
    file_per_column = file_per_column % n_column != 0 ? file_per_column + 1 : file_per_column;

    float vertical_size = total_vertical_size
                          / n_column; // On-screen vertical size of the file list
    vertical_size = vertical_size == 0
                        ? 1
                        : vertical_size; // '0' has a special value for imgui, put '1'

    // Iterator to the file with the biggest file name
    const auto longest_file =
        std::max_element(begin(files), end(files), [](const auto& f1, const auto& f2) {
            return f1.u32filename.size() < f2.u32filename.size();
        });

    int longest_file_size = 0;
    if (longest_file != end(files)) // Makes sure there are files.
    {
        longest_file_size = longest_file->filename.size();
    }
    min_xsize += longest_file_size * hfont_size;
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

void LoadMenu::list_navigation(const std::vector<file_entry>& files,
                               sf::Keyboard::Key key,
                               sf::Uint32 unicode,
                               column_layout layout)
{
    auto [n_column, file_per_column] = layout;

    // Select the appropriate file if the user pressed a key
    if (unicode != 0)
    {
        for (auto i = 0u; i < files.size(); ++i)
        {
            const auto& f = files.at(i);
            if (f.u32filename.size() > 0
                && std::tolower(f.u32filename.at(0)) == std::tolower(unicode))
            {
                file_idx_ = i;
                break;
            }
        }
    }
    if (!files.empty())
    {
        if (key == sf::Keyboard::Key::Right)
        {
            file_idx_ += file_per_column;                // Jump right a column
            if (file_idx_ / file_per_column >= n_column) // If we are at the right edge
            {
                file_idx_ = file_idx_ % file_per_column; // Go to the beginning at the same height
            }
            else if (file_idx_
                     >= (int)files.size()) // We are below the last element in the last column
            {
                file_idx_ = files.size() - 1;
            }
        }
        else if (key == sf::Keyboard::Key::Left)
        {
            const int jump_back = file_idx_ - file_per_column;
            if (jump_back >= 0)
            {
                file_idx_ = jump_back;
            }
            else // We are at the left edge
            {
                // Go to the end at the same height
                file_idx_ = ((n_column - 1) * file_per_column) + file_idx_;
                if (file_idx_ >= (int)files.size()) // past the end
                {
                    file_idx_ = files.size() - 1;
                }
            }
        }
        else if (key == sf::Keyboard::Key::Down)
        {
            ++file_idx_;
            if (file_idx_ > file_per_column * (n_column - 1)) // File is in last column
            {
                if (file_idx_ == (int)files.size()) // If past last element
                {
                    file_idx_ = file_per_column
                                * (n_column - 1); // Go to the top of the last column
                }
            }
            else // File not in last column
            {
                const int pos_in_column = file_idx_ % file_per_column;
                if (pos_in_column % file_per_column == 0)
                {
                    file_idx_ -= file_per_column;
                }
            }
        }
        else if (key == sf::Keyboard::Key::Up)
        {
            if (file_idx_ % file_per_column == 0) // Top of a column
            {
                if (file_idx_ >= file_per_column * (n_column - 1)) // Last column
                {
                    file_idx_ = files.size() - 1; // Last element
                }
                else
                {
                    file_idx_ += file_per_column - 1;
                }
            }
            else
            {
                --file_idx_;
            }
        }
    }
}

void LoadMenu::list(std::vector<file_entry>& files, sf::Keyboard::Key key, sf::Uint32 unicode)
{
    try
    {
        // Get all files in the 'save_dir_' directory, ...
        for (const auto& file : fs::directory_iterator(save_dir_))
        {
            files.push_back(
                {file, file.path().filename().string(), file.path().filename().u32string()});
        }

        // ... remove the directory, links, etc ...
        const auto to_remove = std::remove_if(begin(files), end(files), [](const auto& f) {
            return !fs::is_regular_file(f.file.path());
        });
        files.erase(to_remove, end(files));

        // ... sort them lexicographically, ...
        std::sort(begin(files), end(files), [](const auto& left, const auto& right) {
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

        auto layout = list_layout(files);

        ImGui::Columns(layout.n_column);

        list_navigation(files, key, unicode, layout);


        for (auto i = 0; i < (int)files.size(); ++i)
        {
            const auto& file = files.at(i);

            // Displays the files in a selectable list
            bool selectable_clicked = ImGui::Selectable(file.filename.c_str(), file_idx_ == i);

            if (selectable_clicked && file_idx_ == i && !double_selection_)
            {
                // Double click : the selectable was previously selected,
                double_selection_ = true;
            }
            if (selectable_clicked || file_idx_ == i)
            {
                // If the file was selected by clicking on the Selectable, update the file_idx_
                file_idx_ = i;

                file_to_load_ = string_to_array<FILENAME_LENGTH_>(file.filename);
            }
            if ((i + 1) % layout.file_per_column == 0)
            {
                ImGui::NextColumn();
            }
        }
        ImGui::Columns(1);
        ImGui::EndChild();
    }
    catch (const fs::filesystem_error& e)
    {
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

bool LoadMenu::open(std::list<procgui::LSystemView>& lsys_views,
                    ext::sf::Vector2d load_position,
                    sf::Keyboard::Key key,
                    sf::Uint32 unicode)
{
    ImGui::SetNextWindowPosCenter();
    if (ImGui::Begin("Load LSystem from file",
                     NULL,
                     ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoSavedSettings))
    {
        error_messages.clear();

        std::vector<file_entry> files;

        // Avoid interaction with the background when saving a file.
        ImGui::CaptureKeyboardFromApp();
        ImGui::CaptureMouseFromApp();

        ImGui::Separator();

        list(files, key, unicode);

        ImGui::Separator();

        // Simple informative text
        std::string tmp = "File to load: '" + array_to_string(file_to_load_) + "'";
        ImGui::Text("%s", tmp.c_str());
        ImGui::SameLine();

        load_button(lsys_views, load_position, key);

        ImGui::SameLine();
        ext::ImGui::PushStyleColoredButton<ext::ImGui::Red>();
        if (ImGui::Button("Cancel") || key == sf::Keyboard::Escape)
        {
            close_menu_ = true;
        }
        ImGui::PopStyleColor(3);

        ImGui::End();
    }

    if (close_menu_)
    {
        double_selection_ = false;
        close_menu_ = false;
        return true;
    }

    return false;
}
} // namespace controller
