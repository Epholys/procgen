#include <iostream>
#include <cstdlib>

#include <SFML/Graphics.hpp>

#include "imgui/imgui.h"
#include "imgui/imgui-SFML.h"

#include "LSystem.h"
#include "Turtle.h"
#include "helper_math.h"
#include "procgui.h"

using namespace lsys;
using namespace drawing;
using namespace math;
using namespace procgui;

#ifndef IMGUI_DEMO

// Standard main() for the procgen application
int main(/*int argc, char* argv[]*/)
{
    // if (argc < 2)
    // {
    //     std::cerr << "Usage: procgen [n]" << std::endl;
    //     return 1;;
    // }
    // int n_iter = std::atoi(argv[1]);
    // std::cout << "Number of iterations: " << n_iter << std::endl;

    sf::RenderWindow window(sf::VideoMode(1600, 900), "Procgen");
    window.setVerticalSyncEnabled(true);
    ImGui::SFML::Init(window);
    
    LSystem lsys { "F", { { 'F', "G-F-G" }, { 'G', "F+G+F" } } };
    LSysInterpretation::interpretation_map map { { 'F', go_forward },
                                                 { 'G', go_forward },
                                                 { '+', turn_left  },
                                                 { '-', turn_right } };
    LSysInterpretation interpretation { lsys, map };

    DrawingParameters parameters { { 400, 100 }, 0, degree_to_rad(60), 5 };

    auto vertices = compute_vertices(interpretation, parameters, 7);

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
        display_data(parameters, "Serpinski");
        display_data(lsys, "Serpinski");
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
