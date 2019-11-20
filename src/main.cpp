#include <fstream>
#include <SFML/Graphics.hpp>

#include "imgui/imgui-SFML.h"

#include "LSystemView.h"
#include "RenderWindow.h"
#include "SupplementaryRendering.h"
#include "WindowController.h"
#include "config.h"
#include "PopupGUI.h"

using namespace drawing;
using namespace math;
using namespace procgui;
using namespace controller;
using namespace colors;
using sfml_window::window;

void opt(int argc, char* argv[])
{
    if (argc != 2)
    {
        std::cerr << "Usage: ./procgen x" << std::endl;
        return;
    }
    char arg = argv[1][0];
    if (arg == 'L')
    {
        std::cout << "TESTING LSYSTEM DERIVATION\n";

        LSystem serpinski = LSystem({"F", { { 'F', "G[-F-G" }, { 'G', "F]]+G+F" }}, "G" });

        for (int i = 0; i < 5; ++i) {
            serpinski.add_rule('F', {serpinski.get_rule('F').second+"F"});
            serpinski.produce(10);
            std::cout << serpinski.get_production_cache().at(10).size() << std::endl;
        }
    }
    else if (arg == 'T')
    {
        std::cout << "TESTING VERTICES INTERPRETATION\n";

        LSystem serpinski = LSystem({"F", { { 'F', "G[-F-GFFFF" }, { 'G', "F]]+G+F" }}, "G" });
        DrawingParameters params;
        params.set_n_iter(10);
        auto map = drawing::default_interpretation_map;
        drawing::impl::Turtle turtle (params);

        const auto& [str, rec, _] = serpinski.produce(10);
        for (int i=0; i<5; ++i)
        {
            turtle.compute_vertices(str, rec, params, map);
            std::cout << i << std::endl;
        }
    }
    else if (arg == 'c')
    {
        std::cout << "TESTING VERTEXPAINTER CONSTANT\n";

        LSystem serpinski = LSystem({"F", { { 'F', "G[-F-GFFFF" }, { 'G', "F]]+G+F" }}, "G" });
        const auto& [str, rec, max] = serpinski.produce(10);

        DrawingParameters params;
        auto map = drawing::default_interpretation_map;
        params.set_n_iter(10);
        drawing::impl::Turtle turtle (params);
        ConstantColor cc (sf::Color::Red);
        auto wcc = std::make_shared<ColorGeneratorWrapper>(std::make_shared<ConstantColor>(cc));
        VertexPainterConstant vp (wcc);

        const auto& [vx, vx_iter] = turtle.compute_vertices(str, rec, params, map);
        auto box = geometry::bounding_box(vx);

        std::cout << "BeginPainting\n";
        for (int i=0; i<20; ++i)
        {
            vp.paint_vertices(vx, vx_iter, max, box);
            std::cout << i << std::endl;
        }
    }
    else if (arg == 'l')
    {
        std::cout << "TESTING VERTEXPAINTER LINEAR\n";
        LSystem serpinski = LSystem({"F", { { 'F', "G[-F-GFFFF" }, { 'G', "F]]+G+F" }}, "G" });
        const auto& [str, rec, max] = serpinski.produce(10);

        DrawingParameters params;
        params.set_n_iter(10);

        auto map = drawing::default_interpretation_map;
        drawing::impl::Turtle turtle (params);
        LinearGradient lc({{sf::Color::Red, 0}, {sf::Color::Blue, 1}});
        auto wlc = std::make_shared<ColorGeneratorWrapper>(std::make_shared<LinearGradient>(lc));
        VertexPainterLinear vp (wlc);
        vp.set_angle(25);
        vp.set_display_flag(false);

        const auto& [vx, vx_iter] = turtle.compute_vertices(str, rec, params, map);
        auto box = geometry::bounding_box(vx);

        std::cout << "BeginPainting\n";
        for (int i=0; i<9; ++i)
        {
            vp.paint_vertices(vx, vx_iter, max, box);
            std::cout << i << std::endl;
        }
    }
    else if (arg == 'r')
    {
        std::cout << "TESTING VERTEXPAINTER RADIAL\n";

        LSystem serpinski = LSystem({"F", { { 'F', "G[-F-GFFFF" }, { 'G', "F]]+G+F" }}, "G" });
        const auto& [str, rec, max] = serpinski.produce(10);

        DrawingParameters params;
        params.set_n_iter(10);

        auto map = drawing::default_interpretation_map;

        drawing::impl::Turtle turtle (params);

        LinearGradient lc({{sf::Color::Red, 0}, {sf::Color::Blue, 1}});
        auto wlc = std::make_shared<ColorGeneratorWrapper>(std::make_shared<LinearGradient>(lc));
        VertexPainterRadial vr (wlc);
        vr.set_display_flag(false);

        const auto& [vx, vx_iter] = turtle.compute_vertices(str, rec, params, map);
        auto box = geometry::bounding_box(vx);

        std::cout << "BeginPainting\n";
        for (int i=0; i<9; ++i)
        {
            vr.paint_vertices(vx, vx_iter, max, box);
            std::cout << i << std::endl;
        }
    }
    else if (arg == 'R')
    {
        std::cout << "TESTING VERTEXPAINTER RANDOM\n";

        LSystem serpinski = LSystem({"F", { { 'F', "G[-F-GFFFF" }, { 'G', "F]]+G+F" }}, "G" });
        const auto& [str, rec, max] = serpinski.produce(10);

        DrawingParameters params;
        params.set_n_iter(10);

        auto map = drawing::default_interpretation_map;

        drawing::impl::Turtle turtle (params);

        LinearGradient lc({{sf::Color::Red, 0}, {sf::Color::Blue, 1}});
        auto wlc = std::make_shared<ColorGeneratorWrapper>(std::make_shared<LinearGradient>(lc));
        VertexPainterRandom vr (wlc);
        vr.set_block_size(500);

        const auto& [vx, vx_iter] = turtle.compute_vertices(str, rec, params, map);
        auto box = geometry::bounding_box(vx);

        std::cout << "BeginPainting\n";
        for (int i=0; i<9; ++i)
        {
            vr.paint_vertices(vx, vx_iter, max, box);
            std::cout << i << std::endl;
        }
    }
    else if (arg == 's')
    {
        std::cout << "TESTING VERTEXPAINTER SEQUENTIAL\n";

        LSystem serpinski = LSystem({"F", { { 'F', "G[-F-GFFFF" }, { 'G', "F]]+G+F" }}, "G" });
        const auto& [str, rec, max] = serpinski.produce(10);

        DrawingParameters params;
        params.set_n_iter(10);

        auto map = drawing::default_interpretation_map;

        drawing::impl::Turtle turtle (params);

        LinearGradient lc({{sf::Color::Red, 0}, {sf::Color::Blue, 1}});
        auto wlc = std::make_shared<ColorGeneratorWrapper>(std::make_shared<LinearGradient>(lc));
        VertexPainterSequential vs (wlc);
        vs.set_factor(2);

        const auto& [vx, vx_iter] = turtle.compute_vertices(str, rec, params, map);
        auto box = geometry::bounding_box(vx);

        std::cout << "BeginPainting\n";
        for (int i=0; i<9; ++i)
        {
            vs.paint_vertices(vx, vx_iter, max, box);
            std::cout << i << std::endl;
        }
    }
    else if (arg == 'i')
    {
        std::cout << "TESTING VERTEXPAINTER ITERATION\n";

        LSystem serpinski = LSystem({"F", { { 'F', "G[-F-GFFFF" }, { 'G', "F]]+G+F" }}, "G" });
        const auto& [str, rec, max] = serpinski.produce(10);

        DrawingParameters params;
        params.set_n_iter(10);

        auto map = drawing::default_interpretation_map;

        drawing::impl::Turtle turtle (params);

        LinearGradient lc({{sf::Color::Red, 0}, {sf::Color::Blue, 1}});
        auto wlc = std::make_shared<ColorGeneratorWrapper>(std::make_shared<LinearGradient>(lc));
        VertexPainterIteration vi (wlc);

        const auto& [vx, vx_iter] = turtle.compute_vertices(str, rec, params, map);
        auto box = geometry::bounding_box(vx);

        std::cout << "BeginPainting\n";
        for (int i=0; i<9; ++i)
        {
            vi.paint_vertices(vx, vx_iter, max, box);
            std::cout << i << std::endl;
        }
    }
    else if (arg == 'z')
    {
        std::cout << "TESTING VERTEXPAINTER COMPOSITE\n";

        LSystem serpinski = LSystem({"F", { { 'F', "G[-F-GFFFF" }, { 'G', "F]]+G+F" }}, "G" });
        const auto& [str, rec, max] = serpinski.produce(10);

        DrawingParameters params;
        params.set_n_iter(10);

        auto map = drawing::default_interpretation_map;

        drawing::impl::Turtle turtle (params);

        ConstantColor c (sf::Color::Red);
        std::shared_ptr<VertexPainterWrapper> w =
            std::make_shared<VertexPainterWrapper>(
                std::make_shared<VertexPainterConstant>(
                    std::make_shared<ColorGeneratorWrapper>(
                        std::make_shared<ConstantColor>(c))));
        std::list<std::shared_ptr<VertexPainterWrapper>> depth3 = {w, w, w};

        // auto wc2 =
        //     std::make_shared<VertexPainterWrapper>(
        //         std::make_shared<VertexPainterComposite>());
        // std::dynamic_pointer_cast<VertexPainterComposite>(wc2->unwrap())->set_child_painters(depth3);
        // std::list<std::shared_ptr<VertexPainterWrapper>> depth2 = {wc2, wc2, wc2};

        auto wc1 = std::make_shared<VertexPainterComposite>();
        wc1->set_child_painters(depth3);

        const auto& [vx, vx_iter] = turtle.compute_vertices(str, rec, params, map);
        auto box = geometry::bounding_box(vx);

        std::cout << "BeginPainting\n";
        for (int i=0; i<5; ++i)
        {
            wc1->paint_vertices(vx, vx_iter, max, box);
            std::cout << i << std::endl;
        }
    }
    else if (arg == 'N')
    {
        std::cout << "TESTING COLORGENERATOR LINEAR\n";

        LSystem serpinski = LSystem({"F", { { 'F', "G[-F-GFFFF" }, { 'G', "F]]+G+F" }}, "G" });
        const auto& [str, rec, max] = serpinski.produce(10);

        DrawingParameters params;
        params.set_n_iter(10);

        auto map = drawing::default_interpretation_map;

        drawing::impl::Turtle turtle (params);

        LinearGradient lc({{sf::Color::Red, 0}, {sf::Color::Green, 0.5}, {sf::Color::Yellow, 0.75}, {sf::Color::Blue, 1}});
        auto wlc = std::make_shared<ColorGeneratorWrapper>(std::make_shared<LinearGradient>(lc));
        VertexPainterSequential vs (wlc);

        const auto& [vx, vx_iter] = turtle.compute_vertices(str, rec, params, map);
        auto box = geometry::bounding_box(vx);

        std::cout << "BeginPainting\n";
        for (int i=0; i<9; ++i)
        {
            vs.paint_vertices(vx, vx_iter, max, box);
            std::cout << i << std::endl;
        }
    }
    else if (arg == 'D')
    {
        std::cout << "TESTING COLORGENERATOR DISCRETE\n";

        LSystem serpinski = LSystem({"F", { { 'F', "G[-F-GFFFF" }, { 'G', "F]]+G+F" }}, "G" });
        const auto& [str, rec, max] = serpinski.produce(10);

        DrawingParameters params;
        params.set_n_iter(10);

        auto map = drawing::default_interpretation_map;

        drawing::impl::Turtle turtle (params);

        DiscreteGradient dc({{sf::Color::Red, 0}, {sf::Color::Blue, 500}, {sf::Color::Green, 1000}});
        auto wlc = std::make_shared<ColorGeneratorWrapper>(std::make_shared<DiscreteGradient>(dc));
        VertexPainterSequential vs (wlc);
        vs.set_factor(2);

        const auto& [vx, vx_iter] = turtle.compute_vertices(str, rec, params, map);
        auto box = geometry::bounding_box(vx);

        std::cout << "BeginPainting\n";
        for (int i=0; i<9; ++i)
        {
            vs.paint_vertices(vx, vx_iter, max, box);
            std::cout << i << std::endl;
        }
    }
    else if (arg == 'A')
    {
        std::cout << "TESTING COLORS.lsys\n";

        std::fstream sf ("saves/COLORS.lsys");
        if (!sf.is_open())
        {
            std::cerr << "can't open\n";
            return;
        }

        LSystemView view({0,0},10);
        {
            cereal::JSONInputArchive ar (sf);
            ar(view);
        }

        std::cout << "BeginTESTING\n";
        view.ref_parameters().set_n_iter(9);
        for (int i=0; i<5; ++i)
        {
            auto rule = view.ref_lsystem_buffer().ref_rule_map()->get_rule('F').second + 'F';
            view.ref_lsystem_buffer().ref_rule_map()->add_rule('F', rule);
        }

    }
}

int main(int argc, char* argv[])
{
    if (argc > 1)
    {
        opt(argc, argv);
        return 0;
    }

    // Load config file
    bool load_config_failed = false;
    try
    {
        std::ifstream ifs (config::config_path);
        if (!ifs.is_open())
        {
            load_config_failed = true;
        }
        else
        {
            cereal::JSONInputArchive ar (ifs);
            config::load(ar, 0);
        }
    }
    catch (const cereal::RapidJSONException& e)
    {
        load_config_failed = true;
    }
    if (load_config_failed)
    {
        PopupGUI config_failed =
            {
                "Configuration File Error",
                []()
                {
                    std::string message = "Can't open or load config file: "
                    + config::config_path.string()
                    + "\nDefault configuration will be loaded instead.";
                    ImGui::Text(message.c_str());
                }
            };
        push_popup(config_failed);
    }


    // Init SFML window and imgui
    sfml_window::init_window();
    ImGui::SFML::Init(window);

    // Default L-System
    auto plant = std::make_shared<LSystem>(LSystem { "X", { { 'X', "F[+X][X]F[+X]-FX" }, { 'F', "FF" } }, "X" });
    auto map = std::make_shared<InterpretationMap>(default_interpretation_map);

    auto plant_param = std::make_shared<DrawingParameters>();
    plant_param->set_starting_position({ 400, 800 });
    plant_param->set_starting_angle(degree_to_rad(80.f));
    plant_param->set_delta_angle(degree_to_rad(25.f));
    plant_param->set_n_iter(6);
    plant_param->set_step(4);

    LSystemView plant_view ("Plant", plant, map, plant_param);

    std::list<LSystemView> views;
    views.emplace_back(plant_view);
    views.front().select();
    views.front().finish_loading();


    sf::Clock delta_clock;
    bool last_popup_open = false;
    while (window.isOpen())
    {
        window.clear(sfml_window::background_color);
        SupplementaryRendering::clear_draw_calls();

        std::vector<sf::Event> events;
        sf::Event event;
        // ImGui has the priority as it is the topmost GUI.
        // The events are then redistributed in the rest of the application.
        while(window.pollEvent(event))
        {
            events.push_back(event);
            ImGui::SFML::ProcessEvent(event);
        }

        // TODO remove 'window'
        ImGui::SFML::Update(window, delta_clock.restart());

        WindowController::handle_input(events, views);

        for (auto& v : views)
        {
            v.draw(window);
        }
        SupplementaryRendering::draw(window);

        ImGui::SFML::Render(window);
        window.display();

        if (sfml_window::close_window && !last_popup_open)
        {
            // Save config file.
            bool save_config_failed = false;
            try
            {
                std::ofstream ofs (config::config_path);
                if (!ofs.is_open())
                {
                    save_config_failed = true;
                }
                else
                {
                    cereal::JSONOutputArchive ar (ofs);
                    config::save(ar, 0);
                }
            }
            catch (const cereal::RapidJSONException& e)
            {
                save_config_failed = true;
            }
            if (save_config_failed)
            {
                PopupGUI config_failed =
                    {
                        "Configuration File Error",
                        []()
                        {
                            std::string message = "Can't open or save config file: "
                            + config::config_path.string()
                            + "\nModified configuration will not be saved.";
                            ImGui::Text(message.c_str());
                        }
                    };
                push_popup(config_failed);
            }
            last_popup_open = true;
        }
        if (sfml_window::close_window && popup_empty())
        {
            window.close();
        }
    }

    ImGui::SFML::Shutdown();

    return 0;
}
