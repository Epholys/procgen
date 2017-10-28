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
        if (main) {
            bool is_active = ImGui::Begin(name.c_str());
            if(!is_active) {
                // Window is collapsed, call End();
                ImGui::End();
            }
            return is_active;
        }
         // Otherwise, set up a TreeNode.
        else {
            return ImGui::TreeNode(name.c_str());
        }
    }
    
    // Finish appending to the current window if 'main' is true.
    // Otherwise, close the current TreeNode.
    void conclude(bool main)
    {
        // If we're the main class, stop appending to the current
        // window.
        if (main) {
            ImGui::Separator();
            ImGui::End();
        }
        // Otherwise, close the TreeNode.
        else {
            ImGui::TreePop();
        }
    }
}

namespace ImGui
{
    // Taken from 'imgui.cpp', helper function to display a help tooltip.
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
        if( !set_up(name, main) ) {
            // Early out if the display zone is collapsed.
            return;
        }
    
        {
            ImGui::Text("Axiom:");

            ImGui::Indent();

            // Note :
            // "The copy of the underlying string returned by 'str' is
            // a temporary object that will be destructed at the end
            // of the expression, so directly calling 'c_str()' on the 
            // result of 'str()'
            // (for example in auto '*ptr = out.str().c_str();')
            // results in a dangling pointer."
            // http://en.cppreference.com/w/cpp/io/basic_stringstream/str
            std::ostringstream oss;
            oss << lsys.get_axiom();
            std::string str = oss.str();
            ImGui::Text(str.c_str());

            ImGui::Unindent(); 
        }

        {
            ImGui::Text("Production rules:");

            ImGui::Indent(); 
            for (const auto& rule : lsys.get_rules()) {
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
        if( !set_up(name, main) ) {
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

    // bool interact_with(lsys::LSystem& lsys, const std::string& name, bool main)
    // {
    //     if( !set_up(name, main) ) {
    //         // Early out if the display zone is collapsed.
    //         return;
    //     }
    
    //     {
    //         ImGui::Text("Axiom:");

    //         ImGui::Indent();

    //         // Note :
    //         // "The copy of the underlying string returned by 'str' is
    //         // a temporary object that will be destructed at the end
    //         // of the expression, so directly calling 'c_str()' on the 
    //         // result of 'str()'
    //         // (for example in auto '*ptr = out.str().c_str();')
    //         // results in a dangling pointer."
    //         // http://en.cppreference.com/w/cpp/io/basic_stringstream/str
    //         std::ostringstream oss;
    //         oss << lsys.get_axiom();
    //         std::string str = oss.str();
    //         ImGui::Text(str.c_str());

    //         ImGui::Unindent(); 
    //     }

    //     {
    //         ImGui::Text("Production rules:");

    //         ImGui::Indent(); 
    //         for (const auto& rule : lsys.get_rules()) {
    //             std::ostringstream oss;
    //             oss << rule.first << " -> " << rule.second;
    //             std::string str = oss.str();
    //             ImGui::Text(str.c_str());
    //         }
    //         ImGui::Unindent(); 
    //     }

    //     conclude(main);
    // }

    
    bool interact_with(drawing::DrawingParameters& parameters, const std::string& name, bool main)
    {
        if( !set_up(name, main) ) {
            // Early out if the display zone is collapsed.
            return false;
        }
    
        // Returned value warning if the object was modified
        bool is_modified = false;

        {   
            float pos[2] = { parameters.starting_position.x,
                             parameters.starting_position.y };
            if ( ImGui::DragFloat2("Starting position", pos,
                                   1.f, 0.f, 0.f, "%#.lf") ) {
                is_modified = true;
                parameters.starting_position.x = pos[0];
                parameters.starting_position.y = pos[1];
            }
        }

        {
            float starting_angle_deg = math::rad_to_degree(parameters.starting_angle);
            if ( ImGui::DragFloat("Starting Angle", &starting_angle_deg,
                                  1.f, 0.f, 360.f, "%#.lf") ) {
                is_modified = true;
                parameters.starting_angle = math::degree_to_rad(starting_angle_deg);
            }
        }

        {
            float delta_angle_deg = math::rad_to_degree(parameters.delta_angle);
            if ( ImGui::DragFloat("Angle Delta", &delta_angle_deg,
                                  1.f, 0.f, 360.f, "%#.lf") ) {
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

}

