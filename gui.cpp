#include "gui.h"

using namespace math;

// void show_data(const TYPE& NAME, bool main)
// {
//     if (main) {
//         ImGui::Begin("WIN");
//     }
//     else {
//         ImGui::Text("WIN:");
//         ImGui::Indent();
//     }
//
//
//
//     if (main) {
//         ImGui::End();
//     }
//     else {
//         ImGui::Unindent();
//     }
// }

void show_data(const lsys::LSystem& lsys, bool main)
{
    if (main) {
        ImGui::Begin("LSystem");
    }
    else {
        ImGui::Text("LSystem:");
        ImGui::Indent();
    }
    
    {
        ImGui::Text("Axiom:");

        ImGui::Indent();

        std::stringstream ss;
        ss << lsys.get_axiom();
        ImGui::Text(ss.str().data());

        ImGui::Unindent(); 
    }

    {
        ImGui::Text("Production rules:");

        ImGui::Indent(); 
        for (const auto& rule : lsys.get_rules()) {
            std::stringstream ss;
            ss << rule.first << " -> " << rule.second;
            ImGui::Text(ss.str().data());
        }
        ImGui::Unindent(); 
    }


    if (main) {
        ImGui::End();
    }
    else {
        ImGui::Unindent();
    }
}

void show_data(const logo::Turtle& turtle, bool main)
{
    const int align = 150;   

    if (main) {
        ImGui::Begin("Turtle");
    }
    else {
        ImGui::Text("Turtle:");
        ImGui::Indent();
    }

    {
        ImGui::Text("Starting Position:"); ImGui::SameLine(align);
        ImGui::Text("x: %#.f", turtle.starting_pos.x); ImGui::SameLine();
        ImGui::Text("y: %#.f", turtle.starting_pos.y);
    }

    {
        ImGui::Text("Starting Angle:"); ImGui::SameLine(align);
        ImGui::Text("%#.lf", rad_to_degree(turtle.starting_angle)); ImGui::SameLine();
        ImGui::Text("degree");
    }

    {
        ImGui::Text("Angle Delta:"); ImGui::SameLine(align);
        ImGui::Text("%#.lf", rad_to_degree(turtle.delta_angle)); ImGui::SameLine();
        ImGui::Text("degree");
    }

    {
        ImGui::Text("Step:"); ImGui::SameLine(align);
        ImGui::Text("%d", turtle.step);
    }

    show_data(turtle.lsys);

    if (main) {
        ImGui::End();
    }
    else {
        ImGui::Unindent();
    }
}
