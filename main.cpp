#include <iostream>

#include<SFML/Graphics.hpp>

#include "LSystem.h"
#include "Turtle.h"

using namespace lsys;

int main()
{
    sf::RenderWindow window(sf::VideoMode(1600, 900), "Procgen");
    window.setVerticalSyncEnabled(true);
    
    Turtle turtle;
    turtle.add_order('F', go_forward);
    turtle.add_order('G', go_forward);
    turtle.add_order('+', turn_right);
    turtle.add_order('-', turn_left);
    turtle.compute_vertices(3);
    
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
            window.draw(turtle);
            window.display();
    }
    
    return 0;
}
