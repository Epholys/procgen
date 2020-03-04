#ifndef EXPORT_MENU_H
#define EXPORT_MENU_H

#include "WindowController.h"
#include "modern_cpp.h"

#include <SFML/Window/Keyboard.hpp>
#include <vector>

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
    ~ExportMenu();
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
    void export_lsys();

    const fs::path save_dir_ = WindowController::save_dir_;

    bool first_time_open_ {true};

    bool close_menu_ {false};

    int n_iteration_ {5};
    int image_dim_ {1000};
    double ratio_ {5.f};

    // Ids list of all created popups, existing or deleted.
    std::vector<int> popups_ids_ {};
};
} // namespace controller


#endif // EXPORT_MENU_H
