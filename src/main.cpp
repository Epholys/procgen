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

int main()
{
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
