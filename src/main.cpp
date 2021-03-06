#include "LSystemView.h"
#include "PopupGUI.h"
#include "RenderWindow.h"
#include "SupplementaryRendering.h"
#include "WindowController.h"
#include "config.h"
#include "export.h"
#include "imgui/imgui-SFML.h"

#include <SFML/Graphics.hpp>
#include <fstream>

#ifdef _WIN32 // :(
#    include <windows.h>
#endif

using namespace drawing;
using namespace math;
using namespace procgui;
using namespace controller;
using namespace colors;
using sfml_window::window;

void opt(int argc, char* argv[]);        // NOLINT c-style array
int export_mode(int argc, char* argv[]); // NOLINT c-style array

#ifdef _WIN32
int WINAPI WinMain(HINSTANCE hinstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
#else
int main(int argc, char* argv[])
{
    // if (argc > 1)
    // {
    //     opt(argc, argv);
    //     return 0;
    // }

    if (argc > 1)
    {
        return export_mode(argc, argv);
    }
#endif
    // Load config file
    bool load_config_failed = false;
    try
    {
        std::ifstream ifs(config::config_path);
        if (!ifs.is_open())
        {
            load_config_failed = true;
        }
        else
        {
            cereal::JSONInputArchive ar(ifs);
            config::load(ar, 0);
        }
    }
    catch (const cereal::RapidJSONException& e)
    {
        load_config_failed = true;
    }
    if (load_config_failed)
    {
        PopupGUI config_failed = {
            "Configuration File Error",
            []() {
                std::string message = "Can't open or load config file: "
                                      + config::config_path.string()
                                      + "\nDefault configuration will be loaded instead.";
                ImGui::Text("%s", message.c_str());
            }};
        push_popup(config_failed);
    }


    // Init SFML window and imgui
    sfml_window::init_window();
    ImGui::SFML::Init(window);
    ImGui::GetIO().ConfigFlags |= ImGuiConfigFlags_NoMouseCursorChange;

    // Default L-System
    std::list<LSystemView> views;

    std::ifstream ifs("saves/fern.lsys");
    if (!ifs.is_open())
    {
        LSystem plant {"X", {{'X', "F[+X][-X]F[+X]-FX"}, {'F', "FF"}}, "X"};

        DrawingParameters plant_param({400, 500},
                                      degree_to_rad(80.f),
                                      degree_to_rad(25.f),
                                      10.f,
                                      6);

        ColorGeneratorWrapper constant_color_gen(
            std::make_shared<ConstantColor>(sf::Color(183, 71, 71, 255)));
        VertexPainterWrapper constant_painter(
            std::make_shared<VertexPainterConstant>(constant_color_gen));

        ColorGeneratorWrapper linear_color_gen(
            std::make_shared<LinearGradient>(LinearGradient::keys(
                {{sf::Color(255, 253, 0, 255), 0}, {sf::Color(255, 25, 0, 255), 1}})));
        auto sequential_painter = std::make_shared<VertexPainterSequential>(linear_color_gen);
        sequential_painter->set_factor(5);
        VertexPainterWrapper sequential_painter_wrapper(sequential_painter);

        VertexPainterWrapper main_painter(std::make_shared<VertexPainterIteration>());

        auto composite_painter = std::make_shared<VertexPainterComposite>();
        composite_painter->set_main_painter(main_painter);
        composite_painter->set_child_painters({constant_painter, sequential_painter_wrapper});
        VertexPainterWrapper composite_wrapper(composite_painter);

        LSystemView plant_view("Plant",
                               plant,
                               default_interpretation_map,
                               plant_param,
                               composite_wrapper);
        views.emplace_back(std::move(plant_view));
    }
    else
    {
        procgui::LSystemView loaded_view({400, 500}, WindowController::default_step_);
        {
            cereal::JSONInputArchive ar(ifs);
            ar(loaded_view);
        }
        loaded_view.ref_parameters().set_starting_position({400, 500});
        views.emplace_back(std::move(loaded_view));
    }


    views.back().finish_loading();
    views.back().select();

    sf::Clock delta_clock;
    bool last_popup_open = false;
    while (window.isOpen())
    {
        window.clear(sfml_window::background_color);
        SupplementaryRendering::clear_draw_calls();

        std::vector<sf::Event> events;
        sf::Event event {};
        // ImGui has the priority as it is the topmost GUI.
        // The events are then redistributed in the rest of the application.
        while (window.pollEvent(event))
        {
            events.push_back(event);
            ImGui::SFML::ProcessEvent(event);
        }

        ImGui::SFML::Update(window, delta_clock.restart());

        WindowController::handle_input(events, views);

        for (auto& v : views)
        {
            v.update();
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
                std::ofstream ofs(config::config_path);
                if (!ofs.is_open())
                {
                    save_config_failed = true;
                }
                else
                {
                    cereal::JSONOutputArchive ar(ofs);
                    config::save(ar, 0);
                }
            }
            catch (const cereal::RapidJSONException& e)
            {
                save_config_failed = true;
            }
            if (save_config_failed)
            {
                PopupGUI config_failed = {
                    "Configuration File Error",
                    []() {
                        std::string message = "Can't open or save config file: "
                                              + config::config_path.string()
                                              + "\nModified configuration will not be saved.";
                        ImGui::Text("%s", message.c_str());
                    }};
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

//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

int export_mode(int argc, char* argv[]) // NOLINT c-style array
{
    if (argc != 5)
    {
        std::cout << "Usage: procgen <filename> <n_iter> <image_dim> <ratio>\n";
        return EXIT_SUCCESS;
    }

    std::cout << "WARNING: Exporting is a beta feature.\n";

    const std::string filename = argv[1];
    std::ifstream ifs(filename);
    if (!ifs.is_open())
    {
        std::cerr << "Error: can't open file: " << filename << " ; exiting\n";
        return EXIT_FAILURE;
    }
    procgui::LSystemView view({0, 0}, WindowController::default_step_);
    try
    {
        // Load it from the file.
        cereal::JSONInputArchive archive(ifs);
        archive(view);
    }
    catch (const cereal::RapidJSONException& e)
    {
        std::cerr << "Error: " << filename
                  << " isn't a valid or complete JSON L-System file ; exiting\n";
        return EXIT_FAILURE;
    }


    int n_iter = std::atoi(argv[2]);
    if (n_iter <= 0)
    {
        std::cerr << "Error: <n_iter> is not a valid number ; exiting\n";
        return EXIT_FAILURE;
    }

    int image_dim = std::atoi(argv[3]);
    if (image_dim <= 0)
    {
        std::cerr << "Error: <image_dim> is not a valid number ; exiting\n";
        return EXIT_FAILURE;
    }

    double ratio = std::atof(argv[4]);
    if (ratio <= 0)
    {
        std::cerr << "Error: <ratio> is not a valid number ; exiting\n";
        return EXIT_FAILURE;
    }

    bool success = export_to_png(view, filename + ".png", n_iter, image_dim, ratio);

    if (!success)
    {
        std::cerr << "Error: export failed ; exiting\n";
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}

//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

void opt(int argc, char* argv[]) // NOLINT c-style array
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

        LSystem serpinski = LSystem({"F", {{'F', "G[-F-G"}, {'G', "F]]+G+F"}}, "G"});

        for (int i = 0; i < 5; ++i)
        {
            serpinski.add_rule('F', {serpinski.get_rule('F').second + "F"});
            auto size = drawing::compute_max_size(serpinski,
                                                  drawing::default_interpretation_map,
                                                  10);
            serpinski.produce(10, size.lsystem_size);
            std::cout << serpinski.get_production_cache().at(10).size() << std::endl;
        }
    }
    else if (arg == 'T')
    {
        std::cout << "TESTING VERTICES INTERPRETATION\n";

        LSystem serpinski = LSystem({"F", {{'F', "G[-F-GFFFF"}, {'G', "F]]+G+F"}}, "G"});
        DrawingParameters params;
        params.set_n_iter(10);
        auto map = drawing::default_interpretation_map;
        drawing::Turtle turtle(params);

        auto size = drawing::compute_max_size(serpinski, drawing::default_interpretation_map, 10);
        const auto& [str, rec, _] = serpinski.produce(10, size.lsystem_size);
        for (int i = 0; i < 5; ++i)
        {
            turtle.compute_vertices(str, rec, map, size.vertices_size);
            std::cout << i << std::endl;
        }
    }
    else if (arg == 'c')
    {
        std::cout << "TESTING VERTEXPAINTER CONSTANT\n";

        LSystem serpinski = LSystem({"F", {{'F', "G[-F-GFFFF"}, {'G', "F]]+G+F"}}, "G"});
        auto size = drawing::compute_max_size(serpinski, drawing::default_interpretation_map, 10);
        const auto& [str, rec, max] = serpinski.produce(10, size.lsystem_size);

        DrawingParameters params;
        auto map = drawing::default_interpretation_map;
        params.set_n_iter(10);
        drawing::Turtle turtle(params);
        ConstantColor cc(sf::Color::Red);
        ColorGeneratorWrapper wcc(std::make_shared<ConstantColor>(cc));
        VertexPainterConstant vp(wcc);

        const auto& [vx,
                     vx_iter,
                     vx_tr] = turtle.compute_vertices(str, rec, map, size.vertices_size);
        auto box = geometry::bounding_box(vx);

        std::cout << "BeginPainting\n";
        for (int i = 0; i < 20; ++i)
        {
            vp.paint_vertices(vx, vx_iter, vx_tr, max, box);
            std::cout << i << std::endl;
        }
    }
    else if (arg == 'l')
    {
        std::cout << "TESTING VERTEXPAINTER LINEAR\n";
        LSystem serpinski = LSystem({"F", {{'F', "G[-F-GFFFF"}, {'G', "F]]+G+F"}}, "G"});
        auto size = drawing::compute_max_size(serpinski, drawing::default_interpretation_map, 10);
        const auto& [str, rec, max] = serpinski.produce(10, size.lsystem_size);

        DrawingParameters params;
        params.set_n_iter(10);

        auto map = drawing::default_interpretation_map;
        drawing::Turtle turtle(params);
        LinearGradient lc({{sf::Color::Red, 0}, {sf::Color::Blue, 1}});
        ColorGeneratorWrapper wlc(std::make_shared<LinearGradient>(lc));
        VertexPainterLinear vp(wlc);
        vp.set_angle(25);
        vp.set_display_flag(false);

        const auto& [vx,
                     vx_iter,
                     vx_tr] = turtle.compute_vertices(str, rec, map, size.vertices_size);
        auto box = geometry::bounding_box(vx);

        std::cout << "BeginPainting\n";
        for (int i = 0; i < 9; ++i)
        {
            vp.paint_vertices(vx, vx_iter, vx_tr, max, box);
            std::cout << i << std::endl;
        }
    }
    else if (arg == 'r')
    {
        std::cout << "TESTING VERTEXPAINTER RADIAL\n";

        LSystem serpinski = LSystem({"F", {{'F', "G[-F-GFFFF"}, {'G', "F]]+G+F"}}, "G"});
        auto size = drawing::compute_max_size(serpinski, drawing::default_interpretation_map, 10);
        const auto& [str, rec, max] = serpinski.produce(10, size.lsystem_size);

        DrawingParameters params;
        params.set_n_iter(10);

        auto map = drawing::default_interpretation_map;

        drawing::Turtle turtle(params);

        LinearGradient lc({{sf::Color::Red, 0}, {sf::Color::Blue, 1}});
        ColorGeneratorWrapper wlc(std::make_shared<LinearGradient>(lc));
        VertexPainterRadial vr(wlc);
        vr.set_display_flag(false);

        const auto& [vx,
                     vx_iter,
                     vx_tr] = turtle.compute_vertices(str, rec, map, size.vertices_size);
        auto box = geometry::bounding_box(vx);

        std::cout << "BeginPainting\n";
        for (int i = 0; i < 9; ++i)
        {
            vr.paint_vertices(vx, vx_iter, vx_tr, max, box);
            std::cout << i << std::endl;
        }
    }
    else if (arg == 'R')
    {
        std::cout << "TESTING VERTEXPAINTER RANDOM\n";

        LSystem serpinski = LSystem({"F", {{'F', "G[-F-GFFFF"}, {'G', "F]]+G+F"}}, "G"});
        auto size = drawing::compute_max_size(serpinski, drawing::default_interpretation_map, 10);
        const auto& [str, rec, max] = serpinski.produce(10, size.lsystem_size);

        DrawingParameters params;
        params.set_n_iter(10);

        auto map = drawing::default_interpretation_map;

        drawing::Turtle turtle(params);

        LinearGradient lc({{sf::Color::Red, 0}, {sf::Color::Blue, 1}});
        ColorGeneratorWrapper wlc(std::make_shared<LinearGradient>(lc));
        VertexPainterRandom vr(wlc);
        vr.set_block_size(500);

        const auto& [vx,
                     vx_iter,
                     vx_tr] = turtle.compute_vertices(str, rec, map, size.vertices_size);
        auto box = geometry::bounding_box(vx);

        std::cout << "BeginPainting\n";
        for (int i = 0; i < 9; ++i)
        {
            vr.paint_vertices(vx, vx_iter, vx_tr, max, box);
            std::cout << i << std::endl;
        }
    }
    else if (arg == 's')
    {
        std::cout << "TESTING VERTEXPAINTER SEQUENTIAL\n";

        LSystem serpinski = LSystem({"F", {{'F', "G[-F-GFFFF"}, {'G', "F]]+G+F"}}, "G"});
        auto size = drawing::compute_max_size(serpinski, drawing::default_interpretation_map, 10);
        const auto& [str, rec, max] = serpinski.produce(10, size.lsystem_size);

        DrawingParameters params;
        params.set_n_iter(10);

        auto map = drawing::default_interpretation_map;

        drawing::Turtle turtle(params);

        LinearGradient lc({{sf::Color::Red, 0}, {sf::Color::Blue, 1}});
        ColorGeneratorWrapper wlc(std::make_shared<LinearGradient>(lc));
        VertexPainterSequential vs(wlc);
        vs.set_factor(2);

        const auto& [vx,
                     vx_iter,
                     vx_tr] = turtle.compute_vertices(str, rec, map, size.vertices_size);
        auto box = geometry::bounding_box(vx);

        std::cout << "BeginPainting\n";
        for (int i = 0; i < 9; ++i)
        {
            vs.paint_vertices(vx, vx_iter, vx_tr, max, box);
            std::cout << i << std::endl;
        }
    }
    else if (arg == 'i')
    {
        std::cout << "TESTING VERTEXPAINTER ITERATION\n";

        LSystem serpinski = LSystem({"F", {{'F', "G[-F-GFFFF"}, {'G', "F]]+G+F"}}, "G"});
        auto size = drawing::compute_max_size(serpinski, drawing::default_interpretation_map, 10);
        const auto& [str, rec, max] = serpinski.produce(10, size.lsystem_size);

        DrawingParameters params;
        params.set_n_iter(10);

        auto map = drawing::default_interpretation_map;

        drawing::Turtle turtle(params);

        LinearGradient lc({{sf::Color::Red, 0}, {sf::Color::Blue, 1}});
        ColorGeneratorWrapper wlc(std::make_shared<LinearGradient>(lc));
        VertexPainterIteration vi(wlc);

        const auto& [vx,
                     vx_iter,
                     vx_tr] = turtle.compute_vertices(str, rec, map, size.vertices_size);
        auto box = geometry::bounding_box(vx);

        std::cout << "BeginPainting\n";
        for (int i = 0; i < 9; ++i)
        {
            vi.paint_vertices(vx, vx_iter, vx_tr, max, box);
            std::cout << i << std::endl;
        }
    }
    else if (arg == 'z')
    {
        std::cout << "TESTING VERTEXPAINTER COMPOSITE\n";

        LSystem serpinski = LSystem({"F", {{'F', "G[-F-GFFFF"}, {'G', "F]]+G+F"}}, "G"});
        auto size = drawing::compute_max_size(serpinski, drawing::default_interpretation_map, 10);
        const auto& [str, rec, max] = serpinski.produce(10, size.lsystem_size);

        DrawingParameters params;
        params.set_n_iter(10);

        auto map = drawing::default_interpretation_map;

        drawing::Turtle turtle(params);

        ConstantColor c(sf::Color::Red);
        VertexPainterWrapper w(std::make_shared<VertexPainterConstant>(
            ColorGeneratorWrapper(std::make_shared<ConstantColor>(c))));
        std::vector<VertexPainterWrapper> depth3 = {w, w, w};

        // auto wc2 =
        //     std::make_shared<VertexPainterWrapper>(
        //         std::make_shared<VertexPainterComposite>());
        // std::dynamic_pointer_cast<VertexPainterComposite>(wc2->unwrap())->set_child_painters(depth3);
        // std::list<std::shared_ptr<VertexPainterWrapper>> depth2 = {wc2, wc2, wc2};

        auto wc1 = std::make_shared<VertexPainterComposite>();
        wc1->set_child_painters(depth3);

        const auto& [vx,
                     vx_iter,
                     vx_tr] = turtle.compute_vertices(str, rec, map, size.vertices_size);
        auto box = geometry::bounding_box(vx);

        std::cout << "BeginPainting\n";
        for (int i = 0; i < 5; ++i)
        {
            wc1->paint_vertices(vx, vx_iter, vx_tr, max, box);
            std::cout << i << std::endl;
        }
    }
    else if (arg == 'N')
    {
        std::cout << "TESTING COLORGENERATOR LINEAR\n";

        LSystem serpinski = LSystem({"F", {{'F', "G[-F-GFFFF"}, {'G', "F]]+G+F"}}, "G"});
        auto size = drawing::compute_max_size(serpinski, drawing::default_interpretation_map, 10);
        const auto& [str, rec, max] = serpinski.produce(10, size.lsystem_size);

        DrawingParameters params;
        params.set_n_iter(10);

        auto map = drawing::default_interpretation_map;

        drawing::Turtle turtle(params);

        LinearGradient lc({{sf::Color::Red, 0},
                           {sf::Color::Green, 0.5},
                           {sf::Color::Yellow, 0.75},
                           {sf::Color::Blue, 1}});
        ColorGeneratorWrapper wlc(std::make_shared<LinearGradient>(lc));
        VertexPainterSequential vs(wlc);

        const auto& [vx,
                     vx_iter,
                     vx_tr] = turtle.compute_vertices(str, rec, map, size.vertices_size);
        auto box = geometry::bounding_box(vx);

        std::cout << "BeginPainting\n";
        for (int i = 0; i < 9; ++i)
        {
            vs.paint_vertices(vx, vx_iter, vx_tr, max, box);
            std::cout << i << std::endl;
        }
    }
    else if (arg == 'D')
    {
        std::cout << "TESTING COLORGENERATOR DISCRETE\n";

        LSystem serpinski = LSystem({"F", {{'F', "G[-F-GFFFF"}, {'G', "F]]+G+F"}}, "G"});
        auto size = drawing::compute_max_size(serpinski, drawing::default_interpretation_map, 10);
        const auto& [str, rec, max] = serpinski.produce(10, size.lsystem_size);

        DrawingParameters params;
        params.set_n_iter(10);

        auto map = drawing::default_interpretation_map;

        drawing::Turtle turtle(params);

        DiscreteGradient dc(
            {{sf::Color::Red, 0}, {sf::Color::Blue, 500}, {sf::Color::Green, 1000}});
        ColorGeneratorWrapper wlc(std::make_shared<DiscreteGradient>(dc));
        VertexPainterSequential vs(wlc);
        vs.set_factor(2);

        const auto& [vx,
                     vx_iter,
                     vx_tr] = turtle.compute_vertices(str, rec, map, size.vertices_size);
        auto box = geometry::bounding_box(vx);

        std::cout << "BeginPainting\n";
        for (int i = 0; i < 9; ++i)
        {
            vs.paint_vertices(vx, vx_iter, vx_tr, max, box);
            std::cout << i << std::endl;
        }
    }
    else if (arg == 'A')
    {
        std::cout << "TESTING COLORS.lsys\n";

        std::fstream sf("saves/COLORS.lsys");
        if (!sf.is_open())
        {
            std::cerr << "can't open\n";
            return;
        }

        LSystemView view({0, 0}, 10);
        {
            cereal::JSONInputArchive ar(sf);
            ar(view);
        }

        std::cout << "BeginTESTING\n";
        view.ref_parameters().set_n_iter(9);
        for (int i = 0; i < 5; ++i)
        {
            auto rule = view.ref_lsystem_buffer().ref_rule_map().get_rule('F').second + 'F';
            view.ref_lsystem_buffer().ref_rule_map().add_rule('F', rule);
        }
    }
}
