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
#include "VertexPainterConstant.h"
#include "VertexPainterIteration.h"
#include "VertexPainterRadial.h"
#include "VertexPainterRandom.h"
#include "VertexPainterSequential.h"
#include "VertexPainterComposite.h"

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

    std::shared_ptr<ColorGenerator> cc = std::make_shared<ConstantColor>(sf::Color::Red);
    std::shared_ptr<ColorGenerator> lc = std::make_shared<LinearGradient>(LinearGradient::keys({{sf::Color::Red, 0.}, {sf::Color::Green, 0.25}, {sf::Color::Blue, 0.95}}));
    std::shared_ptr<ColorGenerator> dc = std::make_shared<DiscreteGradient>(DiscreteGradient::keys({{sf::Color::Red, 0}, {sf::Color::Green, 3}, {sf::Color::Blue, 4}}));

    std::shared_ptr<VertexPainterLinear> vpl = std::make_shared<VertexPainterLinear>(lc);
    vpl->set_angle(90.);
    
    std::shared_ptr<VertexPainterConstant> vpc = std::make_shared<VertexPainterConstant>(cc);
    
    std::shared_ptr<VertexPainterSequential> vps = std::make_shared<VertexPainterSequential>(dc);
    vps->set_factor(5.);

    std::shared_ptr<VertexPainterWrapper> main = std::make_shared<VertexPainterWrapper>(vpl);
    std::shared_ptr<VertexPainterWrapper> child1 = std::make_shared<VertexPainterWrapper>(vpc);
    std::shared_ptr<VertexPainterWrapper> child2 = std::make_shared<VertexPainterWrapper>(vps);
    std::list<std::shared_ptr<VertexPainterWrapper>> children {child1, child2};
    
    
    std::shared_ptr<VertexPainterComposite> composite = std::make_shared<VertexPainterComposite>();
    composite->set_main_painter(main);
    composite->set_child_painters(children);    
    std::shared_ptr<VertexPainter> painter= composite;
    {
        cereal::JSONOutputArchive oarchivess(ss);
        oarchivess(cereal::make_nvp("Composite", painter));
    }
    std::cout << ss.str() << std::endl;
    {
        cereal::JSONOutputArchive oarchive(std::cout);
        cereal::JSONInputArchive iarchivess(ss);
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
