#ifndef SAVE_MENU_H
#define SAVE_MENU_H


#include "WindowController.h"
#include "modern_cpp.h"

#include <SFML/Window.hpp>
#include <array>
#include <vector>

namespace controller
{
// A class implementing the save menu of the application.
// Usage : simply call 'open()' and it manages the display and user inputs,
// with the huge help of imgui.
//
// Warning: modifying permission on files or saves/ directory while the menu
// is open is not graceful, but works.
class SaveMenu
{
  public:
    SaveMenu() = default;
    ~SaveMenu();
    // Simplify implementation by deleting these constructors/operator=. If
    // necessary, they will be implemented. For now it avoid wrong behavior.
    SaveMenu(const SaveMenu& other) = delete;
    SaveMenu(SaveMenu&& other) = delete;
    SaveMenu& operator=(const SaveMenu& menu) = delete;
    SaveMenu& operator=(SaveMenu&& menu) = delete;

    // Function called every frame the save menu should be open.
    // Main entry managing display and actions of the save menu.
    //
    // Parameters:
    //   - 'key' the key pressed by the user, if there is one. Used for
    //   keyboard shortcut.
    //   - 'name' the name of the L-System to save. An indication to quickly
    //   save L-System without renaming them.
    //
    // Returns true if the menu has done its job and should be closed
    bool open(sf::Keyboard::Key key, const std::string& name);

  private:
    // A file entry with all its components
    struct file_entry
    {
        fs::directory_entry file;   // The file object
        std::string filename;       // The name in 'char' raw encoding
        std::u32string u32filename; // The name in 'utf32' encoding
    };

    // The column layout of the save manu
    struct column_layout
    {
        int n_column {1};        // Number of column
        int file_per_column {1}; // Number of files per column
    };

    // Compute the layout of the file list display and open it.
    // 'save_files' is used to manages filename sizes.
    static column_layout list_layout(const std::vector<file_entry>& save_files);

    // Display the list and manages selection of files
    // 'save_files' is filled here.
    void list(std::vector<file_entry>& save_files);

    // InputText management
    // 'saves_files' is used to update selected_file_ if the entered name matches.
    void input_text_field(const std::vector<file_entry>& save_files, std::string& trimmed_filename);

    // Save button
    // 'trimmed_filename' is used in the warning popup
    // 'key' for the shortcut save validation
    // Returns true if the LSys should be saved
    bool save_button(const std::string& trimmed_filename, sf::Keyboard::Key& key);

    // Finally, the method to save the LSys.
    // 'trimmed_filename' is the final name.
    void save_lsys(const std::string& trimmed_filename);


    static constexpr int FILENAME_LENGTH_ = WindowController::FILENAME_LENGTH_;
    // File to save to.
    std::array<char, FILENAME_LENGTH_> filename_;
    // Saves directory.
    const fs::path save_dir_ = WindowController::save_dir_;

    // True if the save menu should be closed
    bool close_menu_ {false};

    // Index of the selected file in the list
    int selected_file_ {-1};

    // True if the file was selected by clicking on the list.
    bool click_selected_ {false};
    // True if the file was selected twise (two clicks or text+click)
    bool double_selection_ {false};
    // Flag to warn the InputText the user selected a file in the list (see
    // .cpp for its use)
    bool first_input_frame_ = true;
    // Flag to load 'filename_' from the parameter of 'open()'.
    // Used to quickly save the selected L-System.
    bool first_menu_frame_ = true;

    // Ids list of all created popups, existing or deleted.
    std::vector<int> popups_ids_ {};
};
} // namespace controller


#endif // SAVE_MENU_H
