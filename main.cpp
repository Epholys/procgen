#include <iostream>

#include<SFML/Graphics.hpp>

#include "LSystem.h"
#include "Turtle.h"

using namespace lsys;
using namespace logo;

int main()
{
    sf::RenderWindow window(sf::VideoMode(1600, 900), "Procgen");
    window.setVerticalSyncEnabled(true);
    
    Turtle turtle;
    auto vertices = compute_vertices(turtle, 8);
    
    while (window.isOpen())
    {
            sf::Event event;
            if (window.pollEvent(event))
            {
                    if (event.type == sf::Event::Closed ||
                        (event.type == sf::Event::KeyPressed &&
                         event.key.code == sf::Keyboard::Escape))
                    {
                            window.close();
                    }
            }

            window.clear();
            window.draw(vertices.data(), vertices.size(), sf::LineStrip);
            window.display();
    }
    
    return 0;
}
