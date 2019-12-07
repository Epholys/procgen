#ifndef EXPORT_MENU_H
#define EXPORT_MENU_H

#include <vector>
#include <SFML/Window/Keyboard.hpp>

namespace sf
{
    class Vertex;
}

namespace procgui
{
    class LSystemView;
}

namespace controller
{
    class ExportMenu
    {
    public:

        ExportMenu() = default;
        ~ExportMenu() = default;
        // Simplify implementation by deleting these constructors/operator=. If
        // necessary, they will be implemented. For now it avoid wrong behavior.
        ExportMenu(const ExportMenu& other) = delete;
        ExportMenu(ExportMenu&& other) = delete;
        ExportMenu& operator=(const ExportMenu& menu) = delete;
        ExportMenu& operator=(ExportMenu&& menu) = delete;

        bool open(sf::Keyboard::Key key);

    private:
        void parameters();
        void size_warning() const;
        void export_to_png() const;
        std::vector<sf::Vertex> add_width(const std::vector<sf::Vertex>& v, float w) const;

        bool close_menu_ = false;

        int n_iteration_ = 0;
        int image_dim_ = 1000;
        float ratio_ = 5.f;
    };
}


#endif // EXPORT_MENU_H
