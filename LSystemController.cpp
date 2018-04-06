#include "LSystemController.h"
#include "WindowController.h"

namespace controller
{
    void handle_input_views(std::vector<procgui::LSystemView>& views, const sf::Event& event)
    {
        if (event.type == sf::Event::MouseButtonPressed &&
            event.mouseButton.button == sf::Mouse::Left)
        {
            for (auto& v : views)
            {
                if (v.select(WindowController::real_mouse_position({event.mouseButton.x, event.mouseButton.y})))
                {
                    break;
                }
            }
        }
    }
}
