#include <cctype>
#include <cstring>
#include "procgui.h"

using namespace math;

namespace
{
    // The two next function are shared by all the 'display()' and
    // 'interact_with()' functions. They manage window creation and integration.

    // Open a window named 'name' if 'main' is true.  Otherwise, set up a
    // TreeNode named 'name', to inline the GUI in a existing window.
    // Returns 'false' if the window is collapsed, to early-out.
    bool set_up(const std::string& name, bool main)
    {
       // If we're the main class, open window 'name'.
        if (main)
        {
            bool is_active = ImGui::Begin(name.c_str());
            if(!is_active)
            {
                // Window is collapsed, call End();
                ImGui::End();
            }
            return is_active;
        }
         // Otherwise, set up a TreeNode.
        else
        {
            return ImGui::TreeNode(name.c_str());
        }
    }
    
    // Finish appending to the current window if 'main' is true.
    // Otherwise, close the current TreeNode.
    void conclude(bool main)
    {
        // If we're the main class, stop appending to the current
        // window.
        if (main)
        {
            ImGui::Separator();
            ImGui::End();
        }
        // Otherwise, close the TreeNode.
        else
        {
            ImGui::TreePop();
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
}

namespace procgui
{
    void display(const lsys::LSystem& lsys, const std::string& name, bool main)
    {
        if( !set_up(name, main) )
        {
            // Early out if the display zone is collapsed.
            return;
        }

        {
            ImGui::Text("Axiom:");

            ImGui::Indent();

            ImGui::Text(lsys.get_axiom().c_str());

            ImGui::Unindent(); 
        }

        {
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
        }

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

        {
            ImGui::Text("Starting Position:"); ImGui::SameLine(align);
            ImGui::Text("x: %#.f", parameters.starting_position.x); ImGui::SameLine();
            ImGui::Text("y: %#.f", parameters.starting_position.y);
        }

        {
            ImGui::Text("Starting Angle:"); ImGui::SameLine(align);
            ImGui::Text("%#.lf", math::rad_to_degree(parameters.starting_angle)); ImGui::SameLine();
            ImGui::Text("degree");
        }

        {
            ImGui::Text("Angle Delta:"); ImGui::SameLine(align);
            ImGui::Text("%#.lf", math::rad_to_degree(parameters.delta_angle)); ImGui::SameLine();
            ImGui::Text("degree");
        }

        {
            ImGui::Text("Step:"); ImGui::SameLine(align);
            ImGui::Text("%d", parameters.step);
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
    
        // Returned value warning if the object was modified
        bool is_modified = false;

        {   
            float pos[2] = { parameters.starting_position.x,
                             parameters.starting_position.y };
            if ( ImGui::DragFloat2("Starting position", pos,
                                   1.f, 0.f, 0.f, "%#.lf") )
            {
                is_modified = true;
                parameters.starting_position.x = pos[0];
                parameters.starting_position.y = pos[1];
            }
        }

        {
            float starting_angle_deg = math::rad_to_degree(parameters.starting_angle);
            if ( ImGui::DragFloat("Starting Angle", &starting_angle_deg,
                                  1.f, 0.f, 360.f, "%#.lf") )
            {
                is_modified = true;
                parameters.starting_angle = math::degree_to_rad(starting_angle_deg);
            }
        }

        {
            float delta_angle_deg = math::rad_to_degree(parameters.delta_angle);
            if ( ImGui::DragFloat("Angle Delta", &delta_angle_deg,
                                  1.f, 0.f, 360.f, "%#.lf") )
            {
                is_modified = true;
                parameters.delta_angle = math::degree_to_rad(delta_angle_deg);
            }
        }

        {
            is_modified |= ImGui::DragInt("Step", &parameters.step);
        }

        {
            // Arbitrary value to avoid resource depletion happening with higher
            // number of iterations (several GiB of memory usage and huge CPU
            // load).
            const int n_iter_max = 12;
            is_modified |= ImGui::SliderInt("Iterations", &parameters.n_iter, 0, n_iter_max);
            ImGui::SameLine(); ImGui::ShowHelpMarker("CTRL+click to directly input values. Higher values will use all of your memory and CPU");
        }

        conclude(main);

        return is_modified;
    }

    bool interact_with(LSystemView& lsys_view, const std::string& name, bool main)
    {
        if( !set_up(name, main) )
        {
            // Early out if the display zone is collapsed.
            return false;
        }
        
        bool is_modified = false;
        lsys::LSystem& lsys = lsys_view.lsys;
 
        {
            // TODO: put this in a testable function and use std::array<char>
            // Copy and truncate the 'std::string' axiom into a C-style char
            // array for ImGui's functions.
            char buf[lsys_input_size] = "";
            std::string axiom = lsys.get_axiom();
            axiom.resize(lsys_input_size-1, '\0');
            std::strncpy(buf, axiom.data(), lsys_input_size-1);
            buf[lsys_input_size-1] = '\0';
  
            if (ImGui::InputText("Axiom", buf, lsys_input_size))
            {
                lsys.set_axiom({buf});
                is_modified = true;
            }
        }

        {
            ImGui::Text("Production rules:");

            ImGui::Indent(); 

            auto& rules = lsys_view.rule_buffer;
            using validity    = LSystemView::validity;
            using predecessor = LSystemView::predecessor;
            using successor   = LSystemView::successor;
            bool rules_modified = false;

            
            auto to_delete = rules.end();
            bool add_button = false;
            for (auto it = rules.begin(); it != rules.end(); ++it)
            {
                ImGui::PushID(&(*it));

                ImGui::PushItemWidth(20);
                auto& pred = std::get<predecessor>(*it);
                if (ImGui::InputText("##pred", pred.data(), 2))
                {
                    bool is_duplicate = false;
                    for(auto find_it = rules.begin(); find_it != rules.end(); ++find_it)
                    {
                        if(find_it != it && std::get<predecessor>(*find_it) == pred)
                        {
                            is_duplicate = true;
                            break;
                        }                            
                    }

                    if (is_duplicate)
                    {
                        std::get<validity>(*it) = false;
                    }
                    else
                    {
                        std::get<validity>(*it) = true;
                        rules_modified = true;
                    }
                }

                ImGui::PopItemWidth(); ImGui::SameLine(); ImGui::Text("->"); ImGui::SameLine();

                ImGui::PushItemWidth(200);
                rules_modified |= ImGui::InputText("##succ", std::get<successor>(*it).data(), lsys_input_size);

                ImGui::SameLine();
                if (ImGui::Button("-"))
                {
                    to_delete = it;
                    rules_modified = true;
                }
                if (it == --rules.end())
                {
                    ImGui::SameLine();
                    ImGui::PushStyleColor(ImGuiCol_Button, ImColor::HSV(2/7.0f, 0.6f, 0.6f));
                    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImColor::HSV(2/7.0f, 0.7f, 0.7f));
                    ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImColor::HSV(2/7.0f, 0.8f, 0.8f));
                    if (ImGui::Button("+"))
                    {
                        add_button = true;
                        is_modified = true;
                    }
                    ImGui::PopStyleColor(3);
                }

                if(!std::get<validity>(*it))
                {
                    ImGui::SameLine(); ImGui::TextColored(ImVec4(1.f,0.f,0.f,1.f), "Duplicated predecessor: %s", pred.data());
                }
                
                ImGui::PopID();
            }
            if (to_delete != rules.end())
            {
                rules.erase(to_delete);
            }
            if (add_button)
            {
                predecessor pred;
                pred.fill('\0');
                successor succ;
                succ.fill('\0');
                rules.push_back({true, pred, succ});
            }
            if (rules_modified)
            {
                lsys_view.sync();
                is_modified = true;
            }
        
            
            ImGui::Unindent(); 
        }

        conclude(main);

        return is_modified;
    }
}
