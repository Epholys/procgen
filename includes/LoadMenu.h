#ifndef LOAD_MENU_H
#define LOAD_MENU_H


#include <array>
#include <list>
#include <vector>
#include <filesystem>

#include <SFML/Window.hpp>

#include "WindowController.h"
#include "imgui_extension.h"

namespace procgui
{
    class LSystemView;
}

namespace controller
{
    // A class implementing the load menu of the application.
    // Usage : simply call 'open()' and it manages the display and user inputs,
    // with the huge help of imgui.
    // 
    // Warning: modifying permission on files or saves/ directory while the menu
    // is open is not graceful, but works. 
    class LoadMenu
    {
    public:

        LoadMenu() = default;
        ~LoadMenu();
        // Simplify implementation by deleting these constructors/operator=. If
        // necessary, they will be implemented. For now it avoid wrong behavior.
        LoadMenu(const LoadMenu& other) = delete;
        LoadMenu(LoadMenu&& other) = delete;
        LoadMenu& operator=(const LoadMenu& menu) = delete;
        LoadMenu& operator=(LoadMenu&& menu) = delete;

         // Function called every frame the save menu should be open.
        // Main entry managing display and actions of the save menu.
        //
        // 'lsys_views' is where to save the LSys
        // 'load_position' is the position on screen where to load the LSys
        // 'key' is the one part input management ("Enter" and "Escape")
        // 'unicode' is the other part: used to quick-select the save file on
        // the list 
        // 
        // Returns true if the menu has done its job and should be closed
        bool open(std::list<procgui::LSystemView>& lsys_views,
                  ext::sf::Vector2d load_position,
                  sf::Keyboard::Key key,
                  sf::Uint32 unicode);
        
        
        // Add a error message to be displayed at loading time.
        // This function should be called when an error occurs when
        // deserializing a LSystemView.
        static void add_loading_error_message(const std::string& message);
        
    private:
        // A file entry with all its components
        struct file_entry {
            std::filesystem::directory_entry file; // The file object
            std::string filename;       // The name in 'char' raw encoding
            std::u32string u32filename; // The name in 'utf32' encoding
        };

        // The column layout of the save manu
        struct column_layout
        {
            int n_column { 1 };        // Number of column
            int file_per_column { 1 }; // Number of files per column
        };

        // Compute the layout of the file list display and open it.
        // 'save_files' is used to manages filename sizes.
        column_layout list_layout(const std::vector<file_entry>& files);

        // Update 'file_idx_' according to 'unicode' (quick-selection of the save
        // file with the first same letter) and to 'key' (arrow keys navigation)
        void list_navigation(const std::vector<file_entry>& files,
                             sf::Keyboard::Key key,
                             sf::Uint32 unicode,
                             column_layout layout);

        // Display the list and manages selection of files
        // 'files' is filled here.
        // 'key' and 'unicode' are sent to 'list_navigation'
        void list(std::vector<file_entry>& files,
                  sf::Keyboard::Key key,
                  sf::Uint32 unicode);

        // Display the load button and manages its action.
        // 'key' is for keyboard shortcut.
        // 'load_position' is where to load the LSys
        void load_button(std::list<procgui::LSystemView>& lsys_views,
                         ext::sf::Vector2d load_position,
                         sf::Keyboard::Key& key);

        // Load the LSys in 'file_to_load_' in 'lsys_views' from 'ifs'
        // 'load_position' is where to load the LSys
        void load(std::list<procgui::LSystemView>& lsys_views,
                  ext::sf::Vector2d load_position,
                  std::ifstream& ifs);

        // Just before loading, adjust size and position of the 'view'
        void adjust_lsys(procgui::LSystemView& view,
                         ext::sf::Vector2d load_position);
        

        static constexpr int FILENAME_LENGTH_ = WindowController::FILENAME_LENGTH_;
        // File to save to.
        std::array<char, FILENAME_LENGTH_> file_to_load_;
        // Saves directory.
        const std::filesystem::path save_dir_ = WindowController::save_dir_;        

        // Index of the selected file in the list
        int file_idx_ {0};

        // True if the file was selected twise (two clicks)
        bool double_selection_ { false };
        
        // True if the load menu should be closed
        bool close_menu_ { false };
        
        // Error messages to be displayed if necessary at loading time.
        static std::vector<std::string> error_messages;        

        // Ids list of all created popups, existing or deleted.
        std::vector<int> popups_ids_ {};
    };
}


#endif // LOAD_MENU_H
