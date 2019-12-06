#include <fstream>

#include "imgui/imgui.h"
#include "cereal/archives/json.hpp"

#include "RenderWindow.h"
#include "helper_string.h"
#include "WindowController.h"
#include "LSystemController.h"
#include "LSystemView.h"
#include "SaveMenu.h"
#include "LoadMenu.h"
#include "PopupGUI.h"

using sfml_window::window;
namespace fs = std::filesystem;

namespace controller
{
    sf::View WindowController::view_ {};

    float WindowController::zoom_level_ {1.f};

    sf::Vector2i WindowController::mouse_position_ {};

    bool WindowController::has_focus_ {true};

    bool WindowController::view_can_move_ {false};

    bool WindowController::save_menu_open_ {false};
    bool WindowController::load_menu_open_ {false};
    bool WindowController::quit_popup_open_ { false };

    ext::sf::Vector2d WindowController::load_position_ { 0, 0 };

    const double WindowController::default_step_ {25.f};

    const fs::path WindowController::save_dir_ = fs::u8path(u8"saves");

    SaveMenu WindowController::save_menu_;
    LoadMenu WindowController::load_menu_;


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

    void WindowController::open_save_menu()
    {
        save_menu_open_ = true;
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
                lsys_views.front().select();
            }
            if (ImGui::MenuItem("Load LSystem", "Ctrl+O"))
            {
                load_menu_open_ = true;
                load_position_ = ext::sf::Vector2d(real_mouse_position(sf::Mouse::getPosition(window)));
            }
            ImGui::Separator();
            if (LSystemController::saved_view() && ImGui::MenuItem("Paste", "Ctrl+V"))
            {
                paste_view(lsys_views, LSystemController::saved_view(), real_mouse_position(sf::Mouse::getPosition(window)));
            }
            ImGui::EndPopup();
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
                // Check if a LSystemView is saved but not modified
                quit_popup_open_ = std::any_of(begin(lsys_views), end(lsys_views),
                                               [](const auto& view){return view.is_modified();});
                if (!quit_popup_open_)
                {
                    sfml_window::close_window = true;
                }
                else
                {
                    procgui::PopupGUI quit_popup =
                        { "WARNING##SAVES",
                          []()
                          {
                              ImGui::Text("At least one L-System is not saved.\nDo you still want to quit?");
                          },
                          false, "Yes, quit", "No, stay",
                          []()
                          {
                              sfml_window::close_window = true;
                          }
                        };
                    procgui::push_popup(quit_popup);
                }
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
                    lsys_views.emplace_front(procgui::LSystemView(ext::sf::Vector2d(real_mouse_position({int(sfml_window::window.getSize().x/2),
                                            int(sfml_window::window.getSize().y/2)})),
                            default_step_ * zoom_level_));
                    lsys_views.front().select();
                }
                else if (event.key.code == sf::Keyboard::O)
                {
                    load_menu_open_ = true;
                    load_position_= ext::sf::Vector2d(real_mouse_position(
                                                          {int(sfml_window::window.getSize().x/2),
                                                                  int(sfml_window::window.getSize().y/2)}));

                }
            }

            else if ((load_menu_open_ || save_menu_open_ || !procgui::popup_empty()) &&
                     event.type == sf::Event::KeyPressed)
            {
                key_to_menus = event.key.code;
            }
            else if (load_menu_open_ &&
                     event.type == sf::Event::TextEntered)
            {
                unicode_to_load_window = event.text.unicode;
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

        if (!procgui::popup_empty())
        {
            procgui::display_popups(key_to_menus);
        }
        if (save_menu_open_)
        {
            std::string save_name;

            auto* under_mouse = LSystemController::under_mouse();
            if (under_mouse)
            {
                save_name = under_mouse->get_name();
            }
            save_menu_open_ = !save_menu_.open(key_to_menus, save_name);
        }
        else if (load_menu_open_)
        {
            load_menu_open_ = !load_menu_.open(lsys_views,
                                               load_position_,
                                               key_to_menus,
                                               unicode_to_load_window);
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
