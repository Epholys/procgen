#ifndef SAVE_MENU_H
#define SAVE_MENU_H


#include <array>
#include <vector>
#include <filesystem>

#include <SFML/Window.hpp>

namespace controller
{
    // Warning: modifying permission on files or saves/ directory while the menu is open is not graceful, but works
    class SaveMenu
    {
    public:
        bool open_save_menu(sf::Keyboard::Key key);

    private:
      struct file_entry {
        std::filesystem::directory_entry file;
        std::string filename;
        std::u32string u32filename;
      };

        struct column_layout
        {
            int n_column { 1 };
            int file_per_column { 1 };
        };
        column_layout list_layout(const std::vector<file_entry>& save_files);
        void list(std::vector<file_entry>& save_files);


        // InputText does not put automatically the cursor to the end
        // when selecting a file. As such, we use a InputText callback
        // to put the cursor to the end. The problem is, this callback
        // is executed each frame, but the first one does not count. So
        // we must execute this part for two frames, which is the role
        // of 'first_frame'.
        bool first_input_frame_ = true;
        void input_text_field(const std::vector<file_entry>& save_files, std::string& trimmed_filename);
        bool save_button(const std::string& trimmed_filename, sf::Keyboard::Key& key);
        void save_lsys(const std::string& trimmed_filename);

        
        // TODO common w/ load
        static constexpr int FILENAME_LENGTH_ = 128;
        std::array<char, FILENAME_LENGTH_> filename_;

        // TODO common w/ load
        const std::filesystem::path save_dir_ = std::filesystem::u8path(u8"saves");

        
        int selected_file_ {-1};
        bool click_selected_ { false };
        bool double_selection_ { false };

        bool close_menu_ { false };
    };
}


#endif // SAVE_MENU_H
