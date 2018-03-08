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
    LSystemBuffer serpinski_buffer { serpinski };
    auto plant = std::make_shared<LSystem>(LSystem { "X", { { 'X', "F[-X][X]F[-X]+FX" }, { 'F', "FF" } } });
    LSystemBuffer plant_buffer { plant };
    auto map = std::make_shared<InterpretationMap>(InterpretationMap
                                                     { { 'F', go_forward },
                                                     { 'G', go_forward },
                                                     { '+', turn_left  },
                                                     { '-', turn_right },
                                                     { '[', save_position },
                                                     { ']', load_position } });
    InterpretationMapBuffer map_buffer { map };

    LSystemBuffer lsys_test { serpinski };
    LSystemBuffer lsys_test2 { serpinski };
    InterpretationMapBuffer map_test { map };
    
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

    std::vector<sf::Vertex> v;

    auto serpinski_paths = compute_path(*serpinski, *map, serpinski_param);
    auto plant_paths = compute_path(*plant, *map, plant_param);

    size_t n = std::accumulate(plant_paths.begin(), plant_paths.end(), 0,
                               [](const auto& n, const auto& v) { return n + v.size(); });
    v.reserve(n);
    for(const auto& p : plant_paths) {
        auto vx1 = p.at(0);
        vx1.color = sf::Color(0);
        v.push_back(vx1);
        v.insert(v.end(), p.begin(), p.end());
        auto vx2 = p.at(p.size()-1);
        vx2.color = sf::Color(0);
        v.push_back(vx2);
    }

    sf::Clock delta_clock;
    while (window.isOpen())
    {
        window.clear();
        handle_input(window);
        
        ImGui::SFML::Update(window, delta_clock.restart());

        procgui::new_frame();
        bool is_modified = false;
        is_modified |= interact_with(serpinski_param, "Serpinski");
        // is_modified |= interact_with(serpinski_buffer, "Serpinski");
        // is_modified |= interact_with(map_buffer, "Serpinski");
 
        // is_modified |= interact_with(map_test, "test");
        // is_modified |= interact_with(lsys_test, "test");
        // is_modified |= interact_with(lsys_test, "test");
        // is_modified |= interact_with(plant_param, "Test");
        // is_modified |= interact_with(plant_buffer, "Test");
        // is_modified |= interact_with(map_buffer, "test");
        // is_modified |= interact_with(map_buffer, "Test");
        
        // is_modified |= interact_with(plant_param, "plant");
        // is_modified |= interact_with(plant_buffer, "plant");
        if (is_modified)
        {
            plant_paths = compute_path(*plant, *map, plant_param);
            serpinski_paths = compute_path(*serpinski, *map, serpinski_param);


            size_t n = std::accumulate(plant_paths.begin(), plant_paths.end(), 0,
                                       [](const auto& n, const auto& v) { return n + v.size(); });
            v.clear();
            v.reserve(n);
            for(const auto& p : plant_paths) {
                auto vx1 = p.at(0);
                vx1.color = sf::Color(0);
                v.push_back(vx1);
                v.insert(v.end(), p.begin(), p.end());
                auto vx2 = p.at(p.size()-1);
                vx2.color = sf::Color(0);
                v.push_back(vx2);
            }
        }

        display(*map, "interpretations");

        for(const auto& path : serpinski_paths)
        {
            window.draw(path.data(), path.size(), sf::LineStrip);
        }

        window.draw(v.data(), v.size(), sf::LineStrip);

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
        if(event.type == sf::Event::MouseButtonPressed)
        {
            std::cout << "buttonpressed" << std::endl;
        }
        else if(event.type == sf::Event::GainedFocus)
        {
            std::cout << "focusgained" << std::endl;
        }
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
            
        else if (has_focus && !imgui_io.WantCaptureMouse)
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
                std::cout << "updatepos" << std::endl;
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
            std::cout << "move" << std::endl;
            sf::Vector2i mouse_delta = mouse_position - new_position;
            view.move(sf::Vector2f(mouse_delta) * zoom_level);
            mouse_position = new_position;
        }
    }
    
    sf::RectangleShape rect_mouse ({10.f, 10.f});
    rect_mouse.setPosition(sf::Vector2f(mouse_position));
    rect_mouse.setFillColor(sf::Color::Red);
    window.draw(rect_mouse);
   

    window.setView(view);
}
