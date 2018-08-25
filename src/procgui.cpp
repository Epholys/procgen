#include <cctype>
#include <cstring>
#include <tuple>
#include <chrono>
#include "procgui.h"
#include "helper_string.h"
#include "WindowController.h"
#include "RenderWindow.h"

using namespace math;

namespace procgui
{
    static int call_id = 0;
    void new_frame()
    {
        call_id = 0;
    }
}

namespace
{
    // The two next function are shared by all the 'display()' and
    // 'interact_with()' functions. They manage window creation and integration.

    // Open a window named 'name' if 'main' is true.  Otherwise, set up a
    // TreeNode named 'name', to inline the GUI in a existing window.
    // Returns 'false' if the window is collapsed, to early-out.
    bool set_up(const std::string& name, bool main, bool* open = nullptr)
    {
        auto& id = procgui::call_id;
        ++id;
        
        // If we're the main class, open window 'name'.
        if (main)
        {
            bool is_active = ImGui::Begin(name.c_str(), open, ImGuiWindowFlags_NoSavedSettings);
            if(!is_active)
            {
                // Window is collapsed, call End();
                ImGui::End();
            }
            else
            {
                ImGui::PushID(id);
            }
            return is_active;
        }
        // Otherwise, set up a TreeNode.
        else
        {
            if(ImGui::CollapsingHeader(name.c_str()))
            {
                ImGui::PushID(id);
                return true;
            }
            return false;
        }
    }
    
    // Finish appending to the current window if 'main' is true.
    // Otherwise, close the current TreeNode.
    void conclude(bool main)
    {
        ImGui::PopID();

        // If we're the main class, stop appending to the current
        // window.
        if (main)
        {
            ImGui::Separator();
            ImGui::End();
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
    void display(const LSystem& lsys, const std::string& name, bool main)
    {
        if( !set_up(name, main) )
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

        conclude(main);
    }

    
    void display(const drawing::DrawingParameters& parameters, const std::string& name, bool main)
    {
        if( !set_up(name, main) )
        {
            // Early out if the display zone is collapsed.
            return;
        }
    
        // Arbitrary value to align neatly every members.
        const int align = 150;   

        // --- Starting Position ---
        ImGui::Text("Starting Position:"); ImGui::SameLine(align);
        ImGui::Text("x: %.f", parameters.starting_position.x); ImGui::SameLine();
        ImGui::Text("y: %.f", parameters.starting_position.y);

        // --- Starting Angle ---
        ImGui::Text("Starting Angle:"); ImGui::SameLine(align);
        ImGui::Text("%.lf", math::rad_to_degree(parameters.starting_angle)); ImGui::SameLine();
        ImGui::Text("degree");
            
        // --- Angle Delta ---
        ImGui::Text("Angle Delta:"); ImGui::SameLine(align);
        ImGui::Text("%.lf", math::rad_to_degree(parameters.delta_angle)); ImGui::SameLine();
        ImGui::Text("degree");

        // --- Step ---
        ImGui::Text("Step:"); ImGui::SameLine(align);
        ImGui::Text("%.1lf", parameters.step);

        conclude(main);
    }

    void display(const drawing::InterpretationMap& map, const std::string& name, bool main)
    {
        if( !set_up(name, main) )
        {
            // Early out if the display zone is collapsed.
            return;
        }

        for(auto interpretation : map.get_rules())
        {
            std::string name = interpretation.second.name;
            ImGui::Text("%c -> %s", interpretation.first, name.c_str());

        }
        
        conclude(main);
    }

    

    bool interact_with(drawing::DrawingParameters& parameters, const std::string& name, bool main)
    {
        if( !set_up(name, main) )
        {
            // Early out if the display zone is collapsed.
            return false;
        }
    
        // Returned value informing if the object was modified
        bool is_modified = false;

        // --- Starting position ---
        float pos[2] = { parameters.starting_position.x,
                         parameters.starting_position.y };
        if ( ImGui::DragFloat2("Starting position", pos,
                               1.f, 0.f, 0.f, "%.lf") )
        {
            // is_modified_ is not set: the render state take care of translating the view.
            parameters.starting_position.x = pos[0];
            parameters.starting_position.y = pos[1];
        }

        // --- Starting angle ---
        float starting_angle_deg = math::rad_to_degree(parameters.starting_angle);
        if ( ImGui::DragFloat("Starting Angle", &starting_angle_deg,
                              1.f, 0.f, 360.f, "%.lf") )
        {
            is_modified = true;
            parameters.starting_angle = math::degree_to_rad(starting_angle_deg);
        }

        // --- Angle Delta ---
        float delta_angle_deg = math::rad_to_degree(parameters.delta_angle);
        if ( ImGui::DragFloat("Angle Delta", &delta_angle_deg,
                              1.f, 0.f, 360.f, "%.lf") )
        {
            is_modified = true;
            parameters.delta_angle = math::degree_to_rad(delta_angle_deg);
        }

        // --- Step ---
        is_modified |= ImGui::DragFloat("Step", &parameters.step, 0.2f, 0.f, 0.f, "%#.1lf");

        // --- Iterations ---
        // Arbitrary value to avoid resource depletion happening with higher
        // number of iterations (several GiB of memory usage and huge CPU
        // load).
        const int n_iter_max = 10;
        is_modified |= ImGui::SliderInt("Iterations", &parameters.n_iter, 0, n_iter_max);
        ImGui::SameLine(); ImGui::ShowHelpMarker("CTRL+click to directly input values. Higher values will use all of your memory and CPU");

        conclude(main);

        return is_modified;
    }

    bool interact_with(LSystemBuffer& buffer, const std::string& name, bool main)
    {
        if( !set_up(name, main) )
        {
            // Early out if the display zone is collapsed.
            return false;
        }

        // Returned value informing if the object was modified
        bool is_modified = false;

        // The LSystem itelf
        LSystem& lsys = *buffer.get_target();
        
        // --- Axiom ---
        auto buf = string_to_array<lsys_successor_size>(lsys.get_axiom());
                        
        if (ImGui::InputText("Axiom", buf.data(), lsys_successor_size))
        {
            is_modified = true;
            lsys.set_axiom(array_to_string(buf));
        }

        //  --- Rules ---
        // [ predecessor ] -> [ successor ] [-] (remove rule) | [+] (add rule)

        ImGui::Text("Production rules:");
        is_modified |= interact_with_buffer(buffer,
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

        conclude(main);

        return is_modified;
    }

    bool interact_with(InterpretationMapBuffer& buffer, const std::string& name, bool main)
    {
        if( !set_up(name, main) )
        {
            // Early out
            return false;
        }

        using namespace drawing;
        
        bool is_modified = interact_with_buffer(buffer,
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

        conclude(main);

        return is_modified;
    }

    bool interact_with(std::shared_ptr<colors::ColorGenerator> gen, const std::string& name, bool main)
    {
        if (!set_up(name, main))
        {
            return false;
        }

        bool is_modified = false;    
        auto constant = std::dynamic_pointer_cast<colors::ConstantColor>(gen);
        auto gradient = std::dynamic_pointer_cast<colors::LinearGradient>(gen);
        if (constant)
        {
            is_modified = interact_with(*constant);
        }
        else if (gradient)
        {
            is_modified = interact_with(*gradient);
        }

        conclude(main);
        
        return is_modified;
    }
    bool interact_with(colors::ConstantColor&)
    {
        return false;  // TODO
    }
    bool interact_with(colors::LinearGradient& gen)
    {
        // TODO Comment Document
        bool is_modified = false;
        
        auto keys = gen.get_keys();
        for (unsigned i=0; i<keys.size(); ++i)
        {
            ImGui::PushID(i);
            ImGui::BeginGroup();

            auto& sfcolor = keys.at(i).first;
            ImVec4 imcolor = sfcolor;
            is_modified |= ImGui::ColorEdit4("Color", (float*)&imcolor, ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoLabel | ImGuiColorEditFlags_AlphaPreviewHalf);
            sfcolor = imcolor;
            
            ImGui::PushItemWidth(50);
            is_modified |= ImGui::DragFloat("", &keys.at(i).second, 0.01, 0., 1., "%.2f");
            ImGui::PopItemWidth();

            ImGui::EndGroup();
            ImGui::PopID();
            ImGui::SameLine();
        }

        ImGui::PushStyleGreenButton();
        if (ImGui::Button("+"))
        {
            is_modified = true;
            keys.push_back({sf::Color::White, 1.f});
        }
        ImGui::PopStyleColor(3);
        ImGui::SameLine();
        ImGui::PushStyleRedButton();
        if (ImGui::Button("-") && keys.size() > 2)
        {
            is_modified = true;
            keys.pop_back();
        }
        ImGui::PopStyleColor(3);
        
        auto k = colors::LinearGradient::sanitize_keys(keys);
        ImDrawList* draw_list = ImGui::GetWindowDrawList();
        ImVec2 pos = ImGui::GetCursorScreenPos();
        ImVec2 size {300., 15.};
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
        return is_modified;
    }

    bool interact_with(LSystemView& lsys_view, const std::string& name, bool main, bool* open)
    {
        // To avoid collision with other window of the same name, create a
        // unique ID for the created window. We override the mecanism of
        // same name ==> same window for the LSystemView which is a unique
        // window for a system.
        
        if (open && !(*open))
        {
            return false;
        }
        std::stringstream ss;
        ss << name << "##" << lsys_view.get_id(); // Each window of LSystemView
                                                  // is conserved by its id.
        if (main)
        {
            // Make the window appear at the mouse double-click position with a
            // correct size.
            // Warning: lots of arbitrary values.
            sf::Vector2f pos = sf::Vector2f(controller::WindowController::get_mouse_position());
            pos -= {250,50}; // Shift the window position to make its center
                             // appear a the mouse position.

            // Shift the window position to always appear on-screen in its entirety.
            int windowX = window::window_size.x;
            int windowY = window::window_size.y;
            pos.x = pos.x < 0 ? 0 : pos.x;
            pos.x = pos.x + 500 > windowX ? windowX-500 : pos.x;
            pos.y = pos.y < 0 ? 0 : pos.y;
            pos.y = pos.y + 150 > windowY ? windowY-150 : pos.y;
            ImGui::SetNextWindowPos({pos.x,pos.y}, ImGuiSetCond_Appearing);
            ImGui::SetNextWindowSize({500,150}, ImGuiSetCond_Appearing);

            // The window's title background is set to the unique color
            // associated with the 'lsys_view_'.
            auto color = lsys_view.get_color();
            ImGui::PushStyleColor(ImGuiCol_TitleBg,
                                  static_cast<ImVec4>(ImColor(color.r, color.g, color.b)));
            ImGui::PushStyleColor(ImGuiCol_TitleBgActive,
                                  static_cast<ImVec4>(ImColor(color.r, color.g, color.b)));
        }
        if (!set_up(ss.str(), main, open))
        {
            // Early out if the display zone is collapsed.
            return false;
        }

        // 'is_modified' is true if the DrawingParameter is modified. It does
        // not check the LSystem or the InterpretationMap because the
        // LSystemView is already an Observer of these classes.
        bool is_modified = interact_with(lsys_view.ref_parameters(), "Drawing Parameters", false);
        interact_with(lsys_view.ref_lsystem_buffer(), "LSystem", false);
        interact_with(lsys_view.ref_interpretation_buffer(), "Interpretation Map", false);

        conclude(main);
        if (main)
        {
            ImGui::PopStyleColor(2);
        }
        
        return is_modified;
    }
}
