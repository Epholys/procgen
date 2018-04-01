#include "imgui/imgui.h"
#include "imgui/imgui-SFML.h"
#include "WindowController.h"
#include "LSystemController.h"

namespace controller
{
    void WindowController::handle_input(sf::RenderWindow &window, std::vector<procgui::LSystemView>& views)
    {
        sf::View window_view = window.getView();
        ImGuiIO& imgui_io = ImGui::GetIO();
        sf::Event event;

        while (window.pollEvent(event))
        {
            // ImGui has the priority as it is the topmost GUI.
            ImGui::SFML::ProcessEvent(event);

            // Close the Window if necessary
            if (event.type == sf::Event::Closed ||
                (event.type == sf::Event::KeyPressed &&
                 event.key.code == sf::Keyboard::Escape))
            {
                window.close();
            }

            else if (event.type == sf::Event::GainedFocus)
            {
                has_focus_ = true;
                // Note: the view can not move yet: the old position of the mouse is
                // still in memory, it must be updated.
            }
            else if (event.type == sf::Event::LostFocus)
            {
                has_focus_ = false;
                view_can_move_ = false;
            }
            else if (event.type == sf::Event::Resized)
            {
                window_view.setSize(event.size.width, event.size.height);
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
                        window_view.zoom(0.9f);
                    }
                    else if (delta<0)
                    {
                        zoom_level_ *= 1.1f;
                        window_view.zoom(1.1f);
                    }
                }

                if (event.type == sf::Event::MouseButtonPressed &&
                    event.mouseButton.button == sf::Mouse::Left)
                {
                    // Update the mouse position and finally signal that the view
                    // can now move.
                    mouse_position_ = sf::Mouse::getPosition(window);
                    view_can_move_ = true;
                }
            }

            handle_input_views(views, event, window_view, zoom_level_);
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
                window_view.move(sf::Vector2f(mouse_delta) * zoom_level_);
                mouse_position_ = new_position;
            }
        }
    
        window.setView(window_view);
    }
}
