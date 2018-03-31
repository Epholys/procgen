#include "LSystemController.h"

namespace controller
{
    void handle_input_views(std::vector<procgui::LSystemView>& views, const sf::Event& event)
    {
        if (event.type == sf::Event::MouseButtonPressed &&
            event.mouseButton.button == sf::Mouse::Left)
        {
            for (auto& v : views)
            {
                if (v.select({event.mouseButton.x, event.mouseButton.y}))
                {
                    break;
                }
            }
        }
    }
}
