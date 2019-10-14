#include <SFML/Graphics.hpp>

#include "imgui/imgui-SFML.h"

#include "LSystemView.h"
#include "RenderWindow.h"
#include "SupplementaryRendering.h"

using namespace drawing;
using namespace math;
using namespace procgui;
using namespace controller;
using namespace colors;
using sfml_window::window;

int main(int argc, char* argv[])
{
    if (argc != 2)
    {
        std::cerr << "Usage: ./procgen x" << std::endl;
        return -1;
    }

    char arg = argv[1][0];
        
    if (arg == 'L')
    {
        LSystem serpinski = LSystem({"F", { { 'F', "G[-F-G" }, { 'G', "F]]+G+F" }}, "G" });

        for (int i = 0; i < 5; ++i) {
            serpinski.add_rule('F', {serpinski.get_rule('F').second+"F"});
            serpinski.produce(10);
            std::cout << serpinski.get_production_cache().at(10).size() << std::endl;
        }
    }
    else if (arg == 'T')
    {
        LSystem serpinski = LSystem({"F", { { 'F', "G[-F-GFFFF" }, { 'G', "F]]+G+F" }}, "G" });
        const auto& [str, rec, _] = serpinski.produce(10);

        DrawingParameters params;

        auto map = drawing::default_interpretation_map;

        drawing::impl::Turtle turtle (params, rec);
        
        for (int i=0; i<5; ++i)
        {
            turtle.compute_vertices(str, map);
            std::cout << i << std::endl;
        }
    }
    else if (arg == 'c')
    {
        LSystem serpinski = LSystem({"F", { { 'F', "G[-F-GFFFF" }, { 'G', "F]]+G+F" }}, "G" });
        const auto& [str, rec, max] = serpinski.produce(10);

        DrawingParameters params;

        auto map = drawing::default_interpretation_map;

        drawing::impl::Turtle turtle (params, rec);

        ConstantColor cc (sf::Color::Red);
        auto wcc = std::make_shared<ColorGeneratorWrapper>(std::make_shared<ConstantColor>(cc));
        VertexPainterConstant vp (wcc);
        
        auto tuple = turtle.compute_vertices(str, map);
        auto box = geometry::bounding_box(std::get<0>(tuple));
        
        std::cout << "BeginPainting\n";
        for (int i=0; i<20; ++i)
        {
            vp.paint_vertices(std::get<0>(tuple), std::get<1>(tuple), max, box);
            std::cout << i << std::endl;
        }
    }
    else if (arg == 'l')
    {
        LSystem serpinski = LSystem({"F", { { 'F', "G[-F-GFFFF" }, { 'G', "F]]+G+F" }}, "G" });
        const auto& [str, rec, max] = serpinski.produce(10);

        DrawingParameters params;

        auto map = drawing::default_interpretation_map;

        drawing::impl::Turtle turtle (params, rec);

        LinearGradient lc({{sf::Color::Red, 0}, {sf::Color::Blue, 1}});
        auto wlc = std::make_shared<ColorGeneratorWrapper>(std::make_shared<LinearGradient>(lc));
        VertexPainterLinear vp (wlc);
        vp.set_angle(25);
        vp.set_display_flag(false);
        
        auto tuple = turtle.compute_vertices(str, map);
        auto box = geometry::bounding_box(std::get<0>(tuple));
        
        std::cout << "BeginPainting\n";
        for (int i=0; i<7; ++i)
        {
            vp.paint_vertices(std::get<0>(tuple), std::get<1>(tuple), max, box);
            std::cout << i << std::endl;
        }
    }
    else
    {
        std::cerr << "Usage:\n\tL->LSys stressing" << std::endl;
        return -1;
    }
    
    // sfml_window::init_window();
    // ImGui::SFML::Init(window);

    // // auto serpinski = std::make_shared<LSystem>(LSystem { "F", { { 'F', "G-F-G" }, { 'G', "F+G+F" } } });
    // auto plant = std::make_shared<LSystem>(LSystem { "X", { { 'X', "F[+X][X]F[+X]-FX" }, { 'F', "FF" } }, "X" });
    // // auto fract = std::make_shared<LSystem>(LSystem { "F", { { 'F', "FF-F" } } });
    // auto map = std::make_shared<InterpretationMap>(default_interpretation_map);

    // // auto serpinski_param = std::make_shared<DrawingParameters>();
    // // serpinski_param->set_starting_position({ 1000, 600 });
    // // serpinski_param->set_starting_angle(0.f);
    // // serpinski_param->set_delta_angle(degree_to_rad(60.f));
    // // serpinski_param->set_step(7);
    // // serpinski_param->set_n_iter(5);
    
    // auto plant_param = std::make_shared<DrawingParameters>();
    // plant_param->set_starting_position({ 400, 800 });
    // plant_param->set_starting_angle(degree_to_rad(80.f));
    // plant_param->set_delta_angle(degree_to_rad(25.f));
    // plant_param->set_n_iter(6);
    // plant_param->set_step(4);

    // // auto fract_param = std::make_shared<DrawingParameters>();
    // // fract_param->set_starting_position({ 400., 800. });
    // // fract_param->set_starting_angle(degree_to_rad(0.));
    // // fract_param->set_delta_angle(degree_to_rad(120.));
    // // fract_param->set_step(20.);
    // // fract_param->set_n_iter(10);

    
    // LSystemView plant_view ("Plant", plant, map, plant_param);
    // // LSystemView serpinski_view ("Serpinski", serpinski, map, serpinski_param);
    // // LSystemView fract_view ("Fract", fract, map, fract_param);
    
    // std::list<LSystemView> views;
    // // views.push_back(std::move(serpinski_view));
    // views.push_back(std::move(plant_view));
    // views.front().select();
    // // views.push_back(std::move(fract_view));

    // sf::Clock delta_clock;
    // while (window.isOpen())
    // {
    //     window.clear(sfml_window::background_color);
    //     SupplementaryRendering::clear_draw_calls();

    //     std::vector<sf::Event> events;
    //     sf::Event event;
    //     // ImGui has the priority as it is the topmost GUI.
    //     // The events are then redistributed in the rest of the application.
    //     while(window.pollEvent(event))
    //     {
    //         events.push_back(event); 
    //         ImGui::SFML::ProcessEvent(event);
    //     }

    //     // TODO remove 'window'
    //     ImGui::SFML::Update(window, delta_clock.restart());
        
    //     WindowController::handle_input(events, views);
 
    //     for (auto& v : views)
    //     {
    //         v.draw(window);
    //     }
    //     SupplementaryRendering::draw(window);

    //     ImGui::SFML::Render(window);
    //     window.display();
    // }

    // ImGui::SFML::Shutdown();

    return 0;
}
