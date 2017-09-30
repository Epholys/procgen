#include "procgui.h"

using namespace math;

namespace procgui
{
    bool set_up(const std::string& name, bool main)
    {
        // If we're the main class, open window 'name'.
        if (main) {
            return ImGui::Begin(name.c_str());
        }
        // Otherwise, set up a TreeNode.
        else {
            return ImGui::TreeNode(name.c_str());
        }
    }

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

    void display_data(const lsys::LSystem& lsys, const std::string& name, bool main)
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

    
    void display_data(const drawing::DrawingParameters& parameters, const std::string& name, bool main)
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
}

