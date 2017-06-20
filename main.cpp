#include <iostream>

#include<SFML/Graphics.hpp>

#include "l-system.h"

int main()
{
    // sf::RenderWindow window(sf::VideoMode(1600, 900), "Procgen");

    // while (window.isOpen())
    // {
    //         sf::Event event;
    //         while (window.pollEvent(event))
    //         {
    //                 if (event.type == sf::Event::Closed ||
    //                     (event.type == sf::Event::KeyPressed &&
    //                      event.key.code == sf::Keyboard::Escape))
    //                 {
    //                         window.close();
    //                 }
    //         }

    //         window.clear();
    //         window.display();
    // }

    LSystem system("F", { {'F', "G-F-G"}, {'G', "F+G+F"} });

    std::cout << system.get_axiom() << std::endl;
    for (int i=0; i<5; ++i) {
        std::cout << system.iter() << std::endl;
    }
    
    return 0;
}
