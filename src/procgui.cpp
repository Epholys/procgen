#include <cctype>
#include <cstring>
#include <tuple>
#include <chrono>
#include "procgui.h"
#include "helper_string.h"
#include "WindowController.h"
#include "RenderWindow.h"

using namespace math;

namespace
{
    int embeddedLevel = 0;

    void push_embedded()
    {
        ++embeddedLevel;
    }

    void pop_embedded()
    {
        if (embeddedLevel > 0)
        {
            --embeddedLevel;
        }
    }
    
    // The two next function are shared by all the 'display()' and
    // 'interact_with()' functions. They manage window creation and integration.

    // Open a window named 'name' if 'main' is true.  Otherwise, set up a
    // CollapsingHeader named 'name', to inline the GUI in a existing window.
    // An 'id' can be specified to separate instances *inside* a window.
    // Returns 'false' if the window is collapsed, to early-out.
    bool set_up(const std::string& name,
                bool* open = nullptr)
    {
        // If we're the main class, open window 'name'.
        if (embeddedLevel == 0)
        {
            bool is_active = ImGui::Begin(name.c_str(), open, ImGuiWindowFlags_NoSavedSettings);
            if(!is_active)
            {
                // Window is collapsed, call End();
                ImGui::End();
            }
            return is_active;
        }
        // Otherwise, set up a CollapsingHeader
        else
        {
            bool is_active = ImGui::CollapsingHeader(name.c_str());
            if(is_active)
            {
                ImGui::PushID(name.c_str());
                ImGui::Indent();
            }
            return is_active;
        }
    }
    
    // Finish appending to the current window if 'main' is true.
    // Otherwise, close the current TreeNode.
    void conclude()
    {
         // If we're the main class, stop appending to the current
        // window.
        if (embeddedLevel == 0)
        {
            ImGui::Separator();
            ImGui::End();
        }
        else
        {
            ImGui::Unindent();
            ImGui::PopID();
        }
    }
}

namespace ImGui
{
    // Taken from 'imgui_demo.cpp', helper function to display a help tooltip.
    void ShowHelpMarker(const char* desc)
    {
        ImGui::TextDisabled("(?)");
        if (ImGui::IsItemHovered())
        {
            ImGui::BeginTooltip();
            ImGui::PushTextWrapPos(450.0f);
            ImGui::TextUnformatted(desc);
            ImGui::PopTextWrapPos();
            ImGui::EndTooltip();
        }
    }

    // Taken from 'imgui_demo.cpp': the styles for colored buttons with a style
    // close to the default button.
    // WARNING: Use these with 'ImGui::PopStyleColor(3)'.
    template<int hue> void PushStyleColoredButton()
    {
        ImGui::PushStyleColor(ImGuiCol_Button, static_cast<ImVec4>(ImColor::HSV(hue/7.0f, 0.6f, 0.6f)));
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, static_cast<ImVec4>(ImColor::HSV(hue/7.0f, 0.7f, 0.7f)));
        ImGui::PushStyleColor(ImGuiCol_ButtonActive, static_cast<ImVec4>(ImColor::HSV(hue/7.0f, 0.8f, 0.8f)));
    }
    auto PushStyleRedButton = PushStyleColoredButton<0>;
    auto PushStyleYellowButton = PushStyleColoredButton<1>;
    auto PushStyleGreenButton = PushStyleColoredButton<2>;
    auto PushStyleTurquoiseButton = PushStyleColoredButton<3>;
    auto PushStyleBlueButton = PushStyleColoredButton<4>;
    auto PushStylePurpleButton = PushStyleColoredButton<5>;
    auto PushStylePinkButton = PushStyleColoredButton<6>;
}

namespace procgui
{
    void display(const LSystem& lsys, const std::string& name)
    {
        if( !set_up(name) )
        {
            // Early out if the display zone is collapsed.
            return;
        }

        // --- Axiom ---
        ImGui::Text("Axiom:");

        ImGui::Indent();

        ImGui::Text(lsys.get_axiom().c_str());

        ImGui::Unindent(); 


        // --- Production Rules ---
        ImGui::Text("Production rules:");

        ImGui::Indent(); 
        for (const auto& rule : lsys.get_rules())
        {
            std::ostringstream oss;
            oss << rule.first << " -> " << rule.second;
            std::string str = oss.str();
            ImGui::Text(str.c_str());
        }
        ImGui::Unindent(); 

        conclude();
    }

    
    void display(const drawing::DrawingParameters& parameters, const std::string& name)
    {
        if( !set_up(name) )
        {
            // Early out if the display zone is collapsed.
            return;
        }
    
        // Arbitrary value to align neatly every members.
        const int align = 150;   

        // --- Starting Position ---
        ImGui::Text("Starting Position:"); ImGui::SameLine(align);
        ImGui::Text("x: %.f", parameters.get_starting_position().x); ImGui::SameLine();
        ImGui::Text("y: %.f", parameters.get_starting_position().y);

        // --- Starting Angle ---
        ImGui::Text("Starting Angle:"); ImGui::SameLine(align);
        ImGui::Text("%.lf", math::rad_to_degree(parameters.get_starting_angle())); ImGui::SameLine();
        ImGui::Text("degree");
            
        // --- Angle Delta ---
        ImGui::Text("Angle Delta:"); ImGui::SameLine(align);
        ImGui::Text("%.lf", math::rad_to_degree(parameters.get_delta_angle())); ImGui::SameLine();
        ImGui::Text("degree");

        // --- Step ---
        ImGui::Text("Step:"); ImGui::SameLine(align);
        ImGui::Text("%.1lf", parameters.get_step());

        conclude();
    }

    void display(const drawing::InterpretationMap& map, const std::string& name)
    {
        if( !set_up(name) )
        {
            // Early out if the display zone is collapsed.
            return;
        }

        for(auto interpretation : map.get_rules())
        {
            std::string name = interpretation.second.name;
            ImGui::Text("%c -> %s", interpretation.first, name.c_str());

        }
        
        conclude();
    }

    

    void interact_with(drawing::DrawingParameters& parameters, const std::string& name)
    {
        if( !set_up(name) )
        {
            return;
        }
    
        // --- Starting position ---
        float pos[2] = { parameters.get_starting_position().x,
                         parameters.get_starting_position().y };
        if ( ImGui::DragFloat2("Starting position", pos,
                               1.f, 0.f, 0.f, "%.lf") )
        {
            // is_modified_ is not set: the render state take care of translating the view.
            sf::Vector2f starting_position {pos[0], pos[1]};
            parameters.silently_set_starting_position(starting_position);
        }

        // --- Starting angle ---
        float starting_angle_deg = math::rad_to_degree(parameters.get_starting_angle());
        if ( ImGui::DragFloat("Starting Angle", &starting_angle_deg,
                              1.f, 0.f, 360.f, "%.lf") )
        {
            parameters.set_starting_angle(math::degree_to_rad(starting_angle_deg));
        }

        // --- Angle Delta ---
        float delta_angle_deg = math::rad_to_degree(parameters.get_delta_angle());
        if ( ImGui::DragFloat("Angle Delta", &delta_angle_deg,
                              1.f, 0.f, 360.f, "%.lf") )
        {
            parameters.set_delta_angle(math::degree_to_rad(delta_angle_deg));
        }

        // --- Step ---
        float step = parameters.get_step();
        if(ImGui::DragFloat("Step", &step, 0.2f, 0.f, 0.f, "%#.1lf"))
        {
            parameters.set_step(step);
        }

        // --- Iterations ---
        // Arbitrary value to avoid resource depletion happening with higher
        // number of iterations (several GiB of memory usage and huge CPU
        // load).
        const int n_iter_max = 10;
        int n_iter = parameters.get_n_iter();
        if(ImGui::SliderInt("Iterations", &n_iter, 0, n_iter_max))
        {
            parameters.set_n_iter(n_iter);
        }
        ImGui::SameLine(); ImGui::ShowHelpMarker("CTRL+click to directly input values. Higher values will use all of your memory and CPU");

        conclude();

    }

    void interact_with(LSystemBuffer& buffer, const std::string& name)
    {
        if( !set_up(name) )
        {
            return;
        }

        // The LSystem itelf
        LSystem& lsys = *buffer.get_target();
        
        // --- Axiom ---
        auto buf = string_to_array<lsys_successor_size>(lsys.get_axiom());
                        
        if (ImGui::InputText("Axiom", buf.data(), lsys_successor_size))
        {
            lsys.set_axiom(array_to_string(buf));
        }

        //  --- Rules ---
        // [ predecessor ] -> [ successor ] [-] (remove rule) | [+] (add rule)

        ImGui::Text("Production rules:");
        interact_with_buffer(buffer,
           [&buffer](auto it)
            {
                // Interact with the successor. Except for the input size, does not
                // have any constraints.
                auto array = string_to_array<lsys_successor_size>(it->successor);
                if(ImGui::InputText("##succ", array.data(), lsys_successor_size))
                {
                    buffer.delayed_change_successor(it, array_to_string(array));
                    return true;
                }
                return false;
            });

        conclude();
    }

    void interact_with(InterpretationMapBuffer& buffer, const std::string& name)
    {
        if( !set_up(name) )
        {
            // Early out
            return;
        }

        using namespace drawing;
        
        interact_with_buffer(buffer,
            [&buffer](auto it)
            {
                // ImGui::ListBox needs:
                //   - An array of 'char *' for the different elements
                //   - An index to select between these elements

                // As 'all_orders_name' has the exact same order as
                // 'all_orders', the index is common.
                // 
                // The index is calculated by finding in the vector the order
                // and using the distance between the first element and the
                // current one.
                auto selected_interpretation_it = std::find(all_orders.begin(),
                                                            all_orders.end(),
                                                            it->successor);
                int index = std::distance(all_orders.begin(), selected_interpretation_it);
                if(ImGui::ListBox("##order", &index, all_orders_name.data(), all_orders_name.size()))
                {
                    buffer.delayed_change_successor(it, all_orders.at(index));
                    return true;
                }
                return false;
            });

        conclude();
    }

    void interact_with(colors::VertexPainter& painter, const std::string& name)
    {
        if (!set_up(name))
        {
            return;
        }

        // --- Gradient angle ---
        float angle = painter.get_angle();
        if ( ImGui::DragFloat("Gradient Angle", &angle,
                              1.f, 0.f, 360.f, "%.lf") )
        {
            painter.set_angle(angle);
        }
        
        push_embedded();
        interact_with(*painter.get_generator_buffer(), "Colors");
        pop_embedded();
        
        conclude();
    }
    
    void interact_with(colors::ColorGeneratorBuffer& color_buffer, const std::string& name)
    {
        if (!set_up(name))
        {
            return;
        }

        auto gen = color_buffer.get_generator();
        
        int index = 0;

        const auto& info = typeid(*gen).hash_code();
        if (info == typeid(colors::ConstantColor).hash_code())
        {
            index = 0;
        }
        else if (info == typeid(colors::LinearGradient).hash_code())
        {
            index = 1;
        }
        else if (info == typeid(colors::DiscreteGradient).hash_code())
        {
            index = 2;
        }
        else
        {
            Expects(false);
        }

        const char* generators[3] = {"Constant", "Linear Gradient", "Discrete Gradient"};
        if (ImGui::ListBox("Color Generator", &index, generators, 3))
        {
            if (index == 0)
            {
                gen = std::make_shared<colors::ConstantColor>();
            }
            else if (index == 1)
            {
                gen = std::make_shared<colors::LinearGradient>();
            }
            else
            {
                gen = std::make_shared<colors::DiscreteGradient>();

            }
            color_buffer.set_generator(gen);
        }

        if (index == 0)
        {
            auto constant = std::dynamic_pointer_cast<colors::ConstantColor>(gen);
            interact_with(*constant);
        }
        else if (index == 1)
        {
            auto gradient = std::dynamic_pointer_cast<colors::LinearGradient>(gen);
            interact_with(*gradient);
        }
        else
        {
            auto discrete = std::dynamic_pointer_cast<colors::DiscreteGradient>(gen);
            interact_with(*discrete);
        }

        conclude();
    }
    
    void interact_with(colors::ConstantColor& constant)
    {
        ImVec4 imcolor = constant.get_color();
        if(ImGui::ColorEdit4("Color", (float*)&imcolor, ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoLabel | ImGuiColorEditFlags_AlphaPreviewHalf))
        {
            constant.set_color(imcolor);
        }

        ImDrawList* draw_list = ImGui::GetWindowDrawList();
        ImVec2 pos = ImGui::GetCursorScreenPos();
        ImVec2 size {500., 30.};
        draw_list->AddRectFilled(ImVec2(pos.x, pos.y), ImVec2(pos.x+size.x, pos.y+size.y),
                                 ImGui::ColorConvertFloat4ToU32(imcolor));
        ImGui::Dummy(size);
    }

    void interact_with(colors::LinearGradient& gen)
    {
        bool is_modified = false;
        
        auto keys = gen.get_keys();

        // Modify 'gen''s keys: colors and position
        for (unsigned i=0; i<keys.size(); ++i)
        {
            ImGui::PushID(i);
            ImGui::BeginGroup();

            auto& sfcolor = keys.at(i).first;
            ImVec4 imcolor = sfcolor;
            if(ImGui::ColorEdit4("Color", (float*)&imcolor, ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoLabel | ImGuiColorEditFlags_AlphaPreviewHalf))
            {
                is_modified = true;
            }
            sfcolor = imcolor;
            
            ImGui::PushItemWidth(50);
            if(ImGui::DragFloat("", &keys.at(i).second, 0.01, 0., 1., "%.2f"))
            {
                is_modified = true;
            }
            ImGui::PopItemWidth();

            ImGui::EndGroup();
            ImGui::PopID();
            ImGui::SameLine();
        }

        // Button '+' to add a key.
        ImGui::PushStyleGreenButton();
        if (ImGui::Button("+"))
        {
            is_modified = true;
            keys.push_back({sf::Color::White, 1.f});
        }
        ImGui::PopStyleColor(3);

        // Button '-' to remove a key
        ImGui::PushStyleRedButton();
        if (keys.size() > 2 && (ImGui::SameLine(), ImGui::Button("-")))
        {
            is_modified = true;
            keys.pop_back();
        }
        ImGui::PopStyleColor(3);
        
        auto k = colors::LinearGradient::sanitize_keys(keys);
        // Preview the color gradient
        ImDrawList* draw_list = ImGui::GetWindowDrawList();
        ImVec2 pos = ImGui::GetCursorScreenPos();
        ImVec2 size {500., 30.};
        float x = pos.x;
        float ratio = 0.f;
        for (unsigned i=0; i<k.size()-1; ++i)
        {
            const auto& col1 = k.at(i).first;
            const auto& col2 = k.at(i+1).first;
            const auto& f = k.at(i+1).second;
            draw_list->AddRectFilledMultiColor({x, pos.y}, ImVec2(x+size.x*(f-ratio), pos.y+size.y),
                                               IM_COL32(col1.r, col1.g, col1.b, col1.a),
                                               IM_COL32(col2.r, col2.g, col2.b, col2.a),
                                               IM_COL32(col2.r, col2.g, col2.b, col2.a),
                                               IM_COL32(col1.r, col1.g, col1.b, col1.a));
            ratio = f;
            x = pos.x + size.x*f;
        }

        ImGui::Dummy(size);
        if (is_modified)
        {
            gen.set_keys(keys);
        }
    }

    void interact_with(colors::DiscreteGradient& gen)
    {
        bool is_modified = false;
        
        auto keys = gen.get_keys();

        int modifier = 0;
        // Modify 'gen''s keys: colors and position
        for (unsigned i=0; i<keys.size()-1; ++i)
        {
            ImGui::PushID(i);

            auto& sfcolor = keys.at(i).first;
            ImVec4 imcolor = sfcolor;
            if(ImGui::ColorEdit4("Color", (float*)&imcolor, ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoLabel | ImGuiColorEditFlags_AlphaPreviewHalf))
            {
                is_modified = true;
            }
            sfcolor = imcolor;

            ImGui::SameLine();
            ImGui::BeginGroup();

            int diff = keys.at(i+1).second - keys.at(i).second - 1;
            ImGui::Text(std::to_string(diff).c_str());

            ImGui::PushStyleGreenButton();
            if (ImGui::Button("+"))
            {
                is_modified = true;
                ++modifier;
            }
            ImGui::PopStyleColor(3);
            ImGui::SameLine();
            ImGui::PushStyleRedButton();
            if (diff > 0 && ImGui::Button("-"))
            {
                is_modified = true;
                --modifier;
            }
            ImGui::PopStyleColor(3);

            keys.at(i+1).second += modifier;

            ImGui::EndGroup();
            ImGui::PopID();
            ImGui::SameLine();
        }

        ImGui::PushID(keys.size());
        auto& sfcolor = keys.back().first;
        ImVec4 imcolor = sfcolor;
        if(ImGui::ColorEdit4("Color", (float*)&imcolor, ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoLabel | ImGuiColorEditFlags_AlphaPreviewHalf))
        {
            is_modified = true;
        }
        sfcolor = imcolor;
        ImGui::PopID();

        ImGui::SameLine();
        // Button '+' to add a key.
        ImGui::PushStyleGreenButton();
        if (ImGui::Button("+"))
        {
            is_modified = true;
            keys.push_back({sf::Color::White, keys.back().second+1});
        }
        ImGui::PopStyleColor(3);

        // Button '-' to remove a key
        ImGui::PushStyleRedButton();
        if (keys.size() > 1 && (ImGui::SameLine(), ImGui::Button("-")))
        {
            is_modified = true;
            keys.pop_back();
        }
        ImGui::PopStyleColor(3);
        
        // Preview the color gradient
        std::vector<sf::Color> colors = colors::DiscreteGradient::generate_colors(keys);
        ImDrawList* draw_list = ImGui::GetWindowDrawList();
        ImVec2 pos = ImGui::GetCursorScreenPos();
        ImVec2 size {500., 30.};
        float x = pos.x;
        float width = size.x / colors.size();
        for (const auto& color : colors)
        {
            draw_list->AddRectFilled({x, pos.y}, ImVec2(x+width, pos.y+size.y), IM_COL32(color.r, color.g, color.b, color.a));
            x += width;
        }
        ImGui::Dummy(size);

        if (is_modified)
        {
            gen.set_keys(keys);
        }
    }


    void interact_with(LSystemView& lsys_view, const std::string& name, bool* open)
    {
        if (open && !(*open))
        {
            return;
        }

        // To avoid collision with other window of the same name, create a
        // unique ID for the created window. We override the mecanism of
        // same name ==> same window for the LSystemView which is a unique
        // window for a system.
        std::stringstream ss;
        ss << "##" << lsys_view.get_id(); // Each window of LSystemView
                                          // is conserved by its id.
        if (embeddedLevel == 0)
        {
            // Make the window appear at the mouse double-click position with a
            // correct size.
            // Warning: lots of arbitrary values.
            // Shift the window position to always appear on-screen in its entirety.
            int windowX = window::window_size.x;
            int windowY = window::window_size.y;
            sf::Vector2i pos = controller::WindowController::get_mouse_position();
            auto bounding_box = lsys_view.get_bounding_box();
            if (bounding_box.contains(controller::WindowController::real_mouse_position(pos)))
            {
                auto absolute_right_side = controller::WindowController::absolute_mouse_position({bounding_box.left + bounding_box.width,0});
                pos.x = absolute_right_side.x + 50;
            }
            if (pos.x + 500 > windowX)
            {
                auto absolute_left_side = controller::WindowController::absolute_mouse_position({bounding_box.left,0});
                pos.x = absolute_left_side.x - 550;                
            }
            pos.y -= 150;
            
            pos.x = pos.x < 0 ? 0 : pos.x;
            pos.x = pos.x + 500 > windowX ? windowX-500 : pos.x;
            pos.y = pos.y < 0 ? 0 : pos.y;
            pos.y = pos.y + 450 > windowY ? windowY-450 : pos.y;
            ImGui::SetNextWindowPos(sf::Vector2i{pos.x,pos.y}, ImGuiSetCond_Appearing);
            ImGui::SetNextWindowSize({500,450}, ImGuiSetCond_Appearing);

            // The window's title background is set to the unique color
            // associated with the 'lsys_view_'.
            auto color = lsys_view.get_color();
            ImGui::PushStyleColor(ImGuiCol_TitleBg,
                                  static_cast<ImVec4>(ImColor(color.r, color.g, color.b)));
            ImGui::PushStyleColor(ImGuiCol_TitleBgActive,
                                  static_cast<ImVec4>(ImColor(color.r, color.g, color.b)));

        }
        if (!set_up(name+ss.str(), open))
        {
            if (embeddedLevel == 0)
            {
                ImGui::PopStyleColor(2);
            }
            // Early out if the display zone is collapsed.
            return;
        }

        // 'is_modified' is true if the DrawingParameter is modified. It does
        // not check the LSystem or the InterpretationMap because the
        // LSystemView is already an Observer of these classes.
        push_embedded();
        interact_with(lsys_view.ref_parameters(), "Drawing Parameters"+ss.str());
        interact_with(lsys_view.ref_lsystem_buffer(), "LSystem"+ss.str());
        interact_with(lsys_view.ref_interpretation_buffer(), "Interpretation Map"+ss.str());
        interact_with(lsys_view.ref_vertex_painter(), "Painter");
        pop_embedded();

        conclude();

        if (embeddedLevel == 0)
        {
            ImGui::PopStyleColor(2);
        }
    }
}
