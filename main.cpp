#include <iostream>

#include <SFML/Graphics.hpp>

#include "imgui/imgui.h"
#include "imgui/imgui-SFML.h"

#include "LSystem.h"
#include "Turtle.h"
#include "helper_math.h"
#include "procgui.h"

using namespace lsys;
using namespace logo;
using namespace math;
using namespace procgui;

#ifndef IMGUI_DEMO

// Standard main() for the procgen application
int main()
{
    sf::RenderWindow window(sf::VideoMode(1600, 900), "Procgen");
    window.setVerticalSyncEnabled(true);
    ImGui::SFML::Init(window);
    
    LSystem lsys { "F", { { 'F', "G-F-G" }, { 'G', "F+G+F" } } };

    Turtle::lsys_interpretation intr { { 'F', go_forward },
        { 'G', go_forward },
        { '+', turn_left  },
        { '-', turn_right } };

    Turtle turtle1 { { 400, 100 }, 0, degree_to_rad(60), 5, lsys, intr };
    Turtle turtle2 { { 200, 200 }, 0, degree_to_rad(50), 4, lsys, intr };

    auto vertices = compute_vertices(turtle1, 7);

    sf::Clock deltaClock;
    while (window.isOpen())
    {
        sf::Event event;
        while (window.pollEvent(event))
        {
            ImGui::SFML::ProcessEvent(event);

            if (event.type == sf::Event::Closed ||
                (event.type == sf::Event::KeyPressed &&
                 event.key.code == sf::Keyboard::Escape))
            {
                window.close();
            }
        }

        ImGui::SFML::Update(window, deltaClock.restart());
        window.clear();
        window.draw(vertices.data(), vertices.size(), sf::LineStrip);
        display_data(turtle1, "Turtle1");
        display_data(lsys, "Turtle1");
        display_data(turtle2, "Turtle2");
        ImGui::SFML::Render(window);
        window.display();
    }

    ImGui::SFML::Shutdown();
    
    return 0;
}


#else

// Special main to display imgui's demo
int main()
{
    sf::RenderWindow window(sf::VideoMode(1600, 900), "Procgen");
    window.setVerticalSyncEnabled(true);
    ImGui::SFML::Init(window);
    
    sf::Clock deltaClock;
    while (window.isOpen())
    {
        sf::Event event;
        while (window.pollEvent(event))
        {
            ImGui::SFML::ProcessEvent(event);

            if (event.type == sf::Event::Closed ||
                (event.type == sf::Event::KeyPressed &&
                 event.key.code == sf::Keyboard::Escape))
            {
                window.close();
            }
        }

        ImGui::SFML::Update(window, deltaClock.restart());
        ImGui::ShowTestWindow();
        window.clear();
        ImGui::SFML::Render(window);
        window.display();
    }

    ImGui::SFML::Shutdown();
    
    return 0;
}

#endif
