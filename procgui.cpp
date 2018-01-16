#include <cctype>
#include <cstring>
#include "procgui.h"
#include "helper_string.h"

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

    // Taken from 'imgui_demo.cpp': the styles for colored buttons with a style
    // close to the default button.
    // WARNING: Use these with 'ImGui::PopStyleColor(3)'.
    template<int hue> void PushStyleColoredButton()
    {
        ImGui::PushStyleColor(ImGuiCol_Button, ImColor::HSV(hue/7.0f, 0.6f, 0.6f));
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImColor::HSV(hue/7.0f, 0.7f, 0.7f));
        ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImColor::HSV(hue/7.0f, 0.8f, 0.8f));
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
    void display(const lsys::LSystem& lsys, const std::string& name, bool main)
    {
        if( !set_up(name, main) )
        {
            // Early out if the display zone is collapsed.
            return;
        }

        { // Axiom
            ImGui::Text("Axiom:");

            ImGui::Indent();

            ImGui::Text(lsys.get_axiom().c_str());

            ImGui::Unindent(); 
        }

        { // Production Rules
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

        { // Starting Position
            ImGui::Text("Starting Position:"); ImGui::SameLine(align);
            ImGui::Text("x: %#.f", parameters.starting_position.x); ImGui::SameLine();
            ImGui::Text("y: %#.f", parameters.starting_position.y);
        }

        { // Starting Angle
            ImGui::Text("Starting Angle:"); ImGui::SameLine(align);
            ImGui::Text("%#.lf", math::rad_to_degree(parameters.starting_angle)); ImGui::SameLine();
            ImGui::Text("degree");
        }

        { // Angle Delta
            ImGui::Text("Angle Delta:"); ImGui::SameLine(align);
            ImGui::Text("%#.lf", math::rad_to_degree(parameters.delta_angle)); ImGui::SameLine();
            ImGui::Text("degree");
        }

        { // Step
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
    
        // Returned value informing if the object was modified
        bool is_modified = false;

        { // Starting position
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

        { // Starting angle
            float starting_angle_deg = math::rad_to_degree(parameters.starting_angle);
            if ( ImGui::DragFloat("Starting Angle", &starting_angle_deg,
                                  1.f, 0.f, 360.f, "%#.lf") )
            {
                is_modified = true;
                parameters.starting_angle = math::degree_to_rad(starting_angle_deg);
            }
        }

        { // Angle Delta
            float delta_angle_deg = math::rad_to_degree(parameters.delta_angle);
            if ( ImGui::DragFloat("Angle Delta", &delta_angle_deg,
                                  1.f, 0.f, 360.f, "%#.lf") )
            {
                is_modified = true;
                parameters.delta_angle = math::degree_to_rad(delta_angle_deg);
            }
        }

        { // Step
            is_modified |= ImGui::DragInt("Step", &parameters.step);
        }

        { // Iterations
            // Arbitrary value to avoid resource depletion happening with higher
            // number of iterations (several GiB of memory usage and huge CPU
            // load).
            const int n_iter_max = 10;
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

        // Returned value informing if the object was modified
        bool is_modified = false;

        // The LSystem itelf
        lsys::LSystem& lsys = lsys_view.lsys;
        
        { // Axiom
            auto buf = string_to_array<lsys_successor_size>(lsys.get_axiom());
                        
            if (ImGui::InputText("Axiom", buf.data(), lsys_successor_size))
            {
                is_modified = true;
                lsys.set_axiom(array_to_string(buf));
            }
        }

        { // Production rules
          // | predecessor | -> | successor | [-] (remove rule) | [+] (add rule)

            
            ImGui::Text("Production rules:");

            ImGui::Indent(); 

            auto& rules = lsys_view.rule_buffer_;
            using validity    = LSystemView::validity; // if the rule is unique
            using predecessor = LSystemView::predecessor;
            using successor   = LSystemView::successor;

            // Inform 'lsys_view' to synchronize the rule buffer with the
            // LSystem.
            bool rules_modified = false;

            // Iterator pointing to the rule to delete, if the [-] button is
            // clicked on.
            auto to_delete = rules.end();

            // If the [+] button is clicked we add a rule to the LSystemView.
            bool must_add_rule = false;

            // We use the old iterator style to save the rule to delete, if necessary.
            for (auto it = rules.begin(); it != rules.end(); ++it)
            { 
                auto& rule = *it;
                auto& is_valid = std::get<validity>(rule);
                auto& pred = std::get<predecessor>(rule);
                auto& succ = std::get<successor>(rule);

                ImGui::PushID(&rule); // Create a scope.
                ImGui::PushItemWidth(20);

                // Display the predicate in as InputText
                if (ImGui::InputText("##pred", pred.data(), 2))
                {
                    // The predecessor has been modified by the user. Now...
                    rules_modified = true;
                    
                    // ... check if the new predecessor already exists in the rules.
                    bool is_duplicate = false;
                    for(auto find_it = rules.begin(); find_it != rules.end(); ++find_it)
                    {
                        if(find_it != it && // do not check a rule against itself
                           std::get<predecessor>(*find_it) == pred)
                        {
                            is_duplicate = true;
                            break;
                        }                            
                    }

                    // If the predecessor is not unique, the rule is not valid.
                    is_valid = !is_duplicate;
                }

                ImGui::PopItemWidth(); ImGui::SameLine(); ImGui::Text("->"); ImGui::SameLine();

                ImGui::PushItemWidth(200);

                // Interact with the successor. Except for the input size, does
                // not have any constraints.
                rules_modified |= ImGui::InputText("##succ", succ.data(), lsys_successor_size);

                // The [-] button. If clicked, the current iterator is saved as
                // the one to delete. We reasonably assume a user can not click
                // on two different buttons in the same frame.
                // We will need to synchronize the rules.
                ImGui::SameLine();
                if (ImGui::Button("-"))
                {
                    to_delete = it;
                    rules_modified = true;
                }

                // For the last rule in the buffer, add the [+] button.
                if (it == --rules.end())
                {
                    ImGui::SameLine();
                    ImGui::PushStyleGreenButton();
                    if (ImGui::Button("+"))
                    {
                        // If the button is clicked, we must add a rule to the buffer.
                        must_add_rule = true;
                    }
                    ImGui::PopStyleColor(3);
                }

                // If the current rule is not valid, add a warning.
                if(!is_valid)
                {
                    ImGui::SameLine();
                    ImGui::TextColored(ImVec4(1.f,0.f,0.f,1.f), "Duplicated predecessor: %s", pred.data());
                }
                
                ImGui::PopID(); // End of the loop and the scope
            }

            // Erase the marked rule if necessary
            if (to_delete != rules.end())
            {
                rules.erase(to_delete);
            }

            // Add a rule if necessary
            if (must_add_rule)
            {
                predecessor pred;
                pred.fill('\0');
                successor succ;
                succ.fill('\0');
                rules.push_back({true, pred, succ});
            }

            // Synchronize the rule if necessary
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
