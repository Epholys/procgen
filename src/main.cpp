#include <iostream>
#include <fstream>
#include <cstdlib>

#include <SFML/Graphics.hpp>

#include "imgui/imgui.h"
#include "imgui/imgui-SFML.h"
#include "cereal/archives/json.hpp"

#include "LSystem.h"
#include "RuleMapBuffer.h"
#include "InterpretationMapBuffer.h"
#include "Turtle.h"
#include "helper_math.h"
#include "procgui.h"
#include "WindowController.h"
#include "RenderWindow.h"
#include "ColorsGenerator.h"

using namespace drawing;
using namespace math;
using namespace procgui;
using namespace controller;
using namespace colors;

int main()
{
    sf::RenderWindow window(sf::VideoMode(window::window_size.x, window::window_size.y), "Procgen");
    window.setVerticalSyncEnabled(true);
    ImGui::SFML::Init(window);

    // auto serpinski = std::make_shared<LSystem>(LSystem { "F", { { 'F', "G-F-G" }, { 'G', "F+G+F" } } });
    auto plant = std::make_shared<LSystem>(LSystem { "X", { { 'X', "F[-X][X]F[-X]+FX" }, { 'F', "FF" } }, "X" });
    // auto fract = std::make_shared<LSystem>(LSystem { "F", { { 'F', "FF+F" } } });
    auto map = std::make_shared<InterpretationMap>(default_interpretation_map);

    // auto serpinski_param = std::make_shared<DrawingParameters>();
    // serpinski_param->set_starting_position({ 1000, 600 });
    // serpinski_param->set_starting_angle(0.f);
    // serpinski_param->set_delta_angle(degree_to_rad(60.f));
    // serpinski_param->set_step(7);
    // serpinski_param->set_n_iter(5);
    
    auto plant_param = std::make_shared<DrawingParameters>();
    plant_param->set_starting_position({ 400, 800 });
    plant_param->set_starting_angle(degree_to_rad(80.f));
    plant_param->set_delta_angle(degree_to_rad(25.f));
    plant_param->set_n_iter(6);
    plant_param->set_step(4);

    // auto fract_param = std::make_shared<DrawingParameters>();
    // fract_param->set_starting_position({ 400., 800. });
    // fract_param->set_starting_angle(degree_to_rad(0.));
    // fract_param->set_delta_angle(degree_to_rad(120.));
    // fract_param->set_step(20.);
    // fract_param->set_n_iter(10);

    
    LSystemView plant_view ("Plant", plant, map, plant_param);
    // LSystemView serpinski_view ("Serpinski", serpinski, map, serpinski_param);
    // LSystemView fract_view ("Fract", fract, map, fract_param);
            
    std::stringstream ss;
    std::shared_ptr<ColorGenerator> c = std::make_shared<LinearGradient>(LinearGradient::keys({{sf::Color::Red, 0.}, {sf::Color::Green, 0.25}, {sf::Color::Blue, 0.95}}));
    std::shared_ptr<VertexPainterLinear> o = std::make_shared<VertexPainterLinear>(c);
    o->set_angle(90.);
    std::shared_ptr<VertexPainter> p = o;
    {
        cereal::JSONOutputArchive oarchive(std::cout);
        cereal::JSONOutputArchive oarchivess(ss);
        oarchivess(cereal::make_nvp("Linear", p)); std::cout << std::endl;
        std::cout << ss.str() << std::endl;
    }
    {
        // std::shared_ptr<ColorGenerator> c = std::make_shared<LinearGradient>(LinearGradient::keys({{sf::Color::White, 0}, {sf::Color::Black, 1}}));
        // std::shared_ptr<VertexPainterLinear> p  = std::make_shared<VertexPainterLinear>(c);
        // p->set_angle(90.f);
        // std::shared_ptr<VertexPainter> pp = p;
        cereal::JSONOutputArchive oarchive(std::cout);
        cereal::JSONInputArchive iarchivess(ss);
        std::cout << ss.str() << std::endl;
        // oarchive(pp); std::cout << std::endl;
        std::shared_ptr<VertexPainter> newp;
        iarchivess(newp);
        oarchive(newp); std::cout << std::endl;
    }
    
    std::list<LSystemView> views;
    // views.push_back(std::move(serpinski_view));
    views.push_back(std::move(plant_view));
    // views.push_back(std::move(fract_view));

    sf::Clock delta_clock;
    while (window.isOpen())
    {
        window.clear();

        std::vector<sf::Event> events;
        sf::Event event;
        // ImGui has the priority as it is the topmost GUI.
        // The events are then redistributed in the rest of the application.
        while(window.pollEvent(event))
        {
            events.push_back(event); 
            ImGui::SFML::ProcessEvent(event);
        }
        
        // procgui::new_frame();
        ImGui::SFML::Update(window, delta_clock.restart());
        
        WindowController::handle_input(events, window, views);
 
        for (auto& v : views)
        {
            v.draw(window);
        }
        

        ImGui::SFML::Render(window);
        window.display();
    }

    ImGui::SFML::Shutdown();

    return 0;
}
