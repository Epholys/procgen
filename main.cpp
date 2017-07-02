#include <iostream>

#include <SFML/Graphics.hpp>

#include "imgui/imgui.h"
#include "imgui/imgui-SFML.h"

#include "LSystem.h"
#include "Turtle.h"
#include "helper_math.h"

using namespace lsys;
using namespace logo;
using namespace math;

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

    Turtle turtle { { 400, 100 }, 0, degree_to_rad(60), 5, lsys, intr };

    auto vertices = compute_vertices(turtle, 7);

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

        ImGui::Begin("Hello, world!");
        ImGui::Button("Look at this pretty button");
        ImGui::End();
            
        window.clear();
        window.draw(vertices.data(), vertices.size(), sf::LineStrip);
        ImGui::SFML::Render(window);
        window.display();
    }
    
    return 0;
}
