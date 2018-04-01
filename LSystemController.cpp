#include "LSystemController.h"

namespace controller
{
    void handle_input_views(std::vector<procgui::LSystemView>& views, const sf::Event& event, const sf::View window_view, float zoom_level)
    {
        if (event.type == sf::Event::MouseButtonPressed &&
            event.mouseButton.button == sf::Mouse::Left)
        {
            for (auto& v : views)
            {
                auto size = window_view.getSize();
                auto center = window_view.getCenter();
                sf::Vector2f upright {center.x - size.x/2, center.y - size.y/2};
                sf::Vector2f position {event.mouseButton.x*zoom_level + upright.x,
                                       event.mouseButton.y*zoom_level + upright.y};
                
                if (v.select(position))
                {
                    break;
                }
            }
        }
    }
}
