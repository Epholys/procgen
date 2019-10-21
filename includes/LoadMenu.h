#ifndef LOAD_MENU_H
#define LOAD_MENU_H


#include <array>
#include <list>
#include <vector>
#include <filesystem>

#include <SFML/Window.hpp>

namespace procgui
{
    class LSystemView;
}

namespace controller
{
    // Warning: modifying permission on files or loads/ directory while the menu is open is not graceful, but works
    class LoadMenu
    {
    public:
        bool open_load_menu(std::list<procgui::LSystemView>& lsys_views,
                            sf::Keyboard::Key key,
                            sf::Uint32 unicode);
        
        
        // Add a error message to be displayed at loading time.
        // This function should be called when an error occurs when
        // deserializing a LSystemView.
        static void add_loading_error_message(const std::string& message);
        
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

        column_layout list_layout(const std::vector<file_entry>& files);
        void list_navigation(const std::vector<file_entry> files,
                             sf::Keyboard::Key key,
                             sf::Uint32 unicode,
                             column_layout layout);
        void list(std::vector<file_entry>& files,
                  sf::Keyboard::Key key,
                  sf::Uint32 unicode);

        void load(std::list<procgui::LSystemView>& lsys_views,
                  sf::Keyboard::Key& key);
        void load_lsys(std::list<procgui::LSystemView>& lsys_views,
                       std::ifstream& ifs);
        void adjust_lsys(procgui::LSystemView& view);

        
        // TODO common w/ save
        static constexpr int FILENAME_LENGTH_ = 128;
        std::array<char, FILENAME_LENGTH_> file_to_load_;
        
        // TODO common w/ save
        const std::filesystem::path save_dir_ = std::filesystem::u8path(u8"saves");
        
        
        int file_idx_ {0};
        
        bool double_selection_ { false };
        
        bool close_menu_ { false };
        
        // Error messages to be displayed if necessary at loading time.
        static std::vector<std::string> error_messages;        
    };
}


#endif // LOAD_MENU_H
