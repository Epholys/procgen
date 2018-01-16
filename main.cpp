#include <iostream>
#include <cstdlib>

#include <SFML/Graphics.hpp>

#include "imgui/imgui.h"
#include "imgui/imgui-SFML.h"

#include "LSystem.h"
#include "LSystemView.h"
#include "Turtle.h"
#include "helper_math.h"
#include "procgui.h"

#include <functional>

using namespace lsys;
using namespace drawing;
using namespace math;
using namespace procgui;

// Forward Declaration
void handle_input(sf::RenderWindow& window);

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

    LSystem serpinski { "F", { { 'F', "G-F-G" }, { 'G', "F+G+F" } } };
    LSystemView serpinski_view { serpinski };
    LSystem smally    { "FG", { { 'F', "F+G" }, { 'G', "G-F" } } };
    LSystemView smally_view { smally };
    interpretation_map map  = { { 'F', go_forward },
                                { 'G', go_forward },
                                { '+', turn_left  },
                                { '-', turn_right } };

    DrawingParameters serpinski_param;
    serpinski_param.starting_position = { 400, 100 };
    serpinski_param.starting_angle = 0.f;
    serpinski_param.delta_angle = degree_to_rad(60.f);
    serpinski_param.step = 5;
    serpinski_param.n_iter = 7;

    DrawingParameters smally_param;
    smally_param.starting_position = { 1000, 600 };
    smally_param.starting_angle = 0.f;
    smally_param.delta_angle = degree_to_rad(60.f);
    smally_param.step = 10;
    smally_param.n_iter = 4;

    auto serpinski_vertices = compute_vertices(serpinski, map, serpinski_param);
    auto smally_vertices = compute_vertices(smally, map, smally_param);

    sf::Clock delta_clock;
    while (window.isOpen())
    {
        handle_input(window);
        
        ImGui::SFML::Update(window, delta_clock.restart());

        window.clear();
        bool is_modified = false;
        is_modified |= interact_with(serpinski_param, "Serpinski");
        is_modified |= interact_with(serpinski_view, "Serpinski");
        if (is_modified)
        {
            serpinski_vertices = compute_vertices(serpinski, map, serpinski_param);
        }
        display(serpinski, "Serpinski");

        is_modified |= interact_with(smally_param, "smally");
        is_modified |= interact_with(smally_view, "smally");
        if (is_modified)
        {
            smally_vertices = compute_vertices(smally, map, smally_param);
        }

        window.draw(serpinski_vertices.data(), serpinski_vertices.size(), sf::LineStrip);
        window.draw(smally_vertices.data(), smally_vertices.size(), sf::LineStrip);
        ImGui::SFML::Render(window);
        window.display();
    }

    ImGui::SFML::Shutdown();

    return 0;
}

// Handle input with SFML and ImGui
void handle_input(sf::RenderWindow& window)
{
    static float zoom_level = 1.f;
    static sf::Vector2i mouse_position {};
    static bool has_focus = true;
    
    sf::View view = window.getView();
    ImGuiIO& imgui_io = ImGui::GetIO();
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

        else if (event.type == sf::Event::GainedFocus)
        {
            has_focus = true;
        }
        else if (event.type == sf::Event::LostFocus)
        {
            has_focus = false;
        }
        else if (event.type == sf::Event::Resized)
        {
            view.setSize(event.size.width, event.size.height);
        }
            
        else if (!imgui_io.WantCaptureMouse)
        {
            if (event.type == sf::Event::MouseWheelMoved)
            {
                auto delta = event.mouseWheel.delta;
                if (delta>0)
                {
                    zoom_level *= 0.9f;
                    view.zoom(0.9f);
                }
                else if (delta<0)
                {
                    zoom_level *= 1.1f;
                    view.zoom(1.1f);
                }
            }

            else if (event.type == sf::Event::MouseButtonPressed &&
                     event.mouseButton.button == sf::Mouse::Left)
            {
                mouse_position = sf::Mouse::getPosition(window);
            }
        }
    }

    if (has_focus &&
        !imgui_io.WantCaptureMouse &&
        sf::Mouse::isButtonPressed(sf::Mouse::Left))
    {
        sf::Vector2i new_position = sf::Mouse::getPosition(window);
        sf::IntRect window_rect (sf::Vector2i(0,0), sf::Vector2i(window.getSize()));
        if (window_rect.contains(new_position))
        {
            sf::Vector2i mouse_delta = mouse_position - new_position;
            view.move(sf::Vector2f(mouse_delta) * zoom_level);
            mouse_position = new_position;
        }
    }

    window.setView(view);
}
