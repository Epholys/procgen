#include <iostream>
#include <cstdlib>

#include <SFML/Graphics.hpp>

#include "imgui/imgui.h"
#include "imgui/imgui-SFML.h"

#include "LSystem.h"
#include "RuleMapBuffer.h"
#include "InterpretationMapBuffer.h"
#include "Turtle.h"
#include "helper_math.h"
#include "procgui.h"

#include <functional>

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

    auto serpinski = std::make_shared<LSystem>(LSystem { "F", { { 'F', "G-F-G" }, { 'G', "F+G+F" } } });
    auto plant = std::make_shared<LSystem>(LSystem { "X", { { 'X', "F[-X][X]F[-X]+FX" }, { 'F', "FF" } } });
    auto map = std::make_shared<InterpretationMap>(InterpretationMap
                                                     { { 'F', go_forward },
                                                     { 'G', go_forward },
                                                     { '+', turn_left  },
                                                     { '-', turn_right },
                                                     { '[', save_position },
                                                     { ']', load_position } });
    DrawingParameters serpinski_param;
    serpinski_param.starting_position = { 1000, 600 };
    serpinski_param.starting_angle = 0.f;
    serpinski_param.delta_angle = degree_to_rad(60.f);
    serpinski_param.step = 7;
    serpinski_param.n_iter = 5;

    DrawingParameters plant_param;
    plant_param.starting_position = { 400, 800 };
    plant_param.starting_angle = degree_to_rad(-80.f);
    plant_param.delta_angle = degree_to_rad(25.f);
    plant_param.step = 5;
    plant_param.n_iter = 6;

    LSystemView serpinski_view (serpinski, map, serpinski_param);
    LSystemView plant_view (plant, map, plant_param);

    sf::Clock delta_clock;
    while (window.isOpen())
    {
        window.clear();
        handle_input(window);
        
        ImGui::SFML::Update(window, delta_clock.restart());

        procgui::new_frame();
        
        serpinski_view.draw(window);
        plant_view.draw(window);

        display(*map, "interpretations");
        
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
    static bool can_move = false;
    
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
            can_move = false;
        }
        else if (event.type == sf::Event::Resized)
        {
            view.setSize(event.size.width, event.size.height);
        }
            
        else if (has_focus) // && !imgui_io.WantCaptureMouse)
        {
            if(!imgui_io.WantCaptureMouse &&
                event.type == sf::Event::MouseWheelMoved)
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

            if (event.type == sf::Event::MouseButtonPressed &&
                event.mouseButton.button == sf::Mouse::Left)
            {
                mouse_position = sf::Mouse::getPosition(window);
                can_move = true;
            }
        }
    }

    if (has_focus && can_move &&
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
