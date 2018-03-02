#include <cctype>
#include <cstring>
#include <tuple>
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
    void display(const LSystem& lsys, const std::string& name, bool main)
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

    void display(const drawing::InterpretationMap& map, const std::string& name, bool main)
    {
        if( !set_up(name, main) )
        {
            // Early out if the display zone is collapsed.
            return;
        }

        for(auto interpretation : map)
        {
            std::string name = get_order_entry(interpretation.second).name;
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
        LSystem& lsys = buffer.get_lsys();
        
        { // Axiom
            auto buf = string_to_array<lsys_successor_size>(lsys.get_axiom());
                        
            if (ImGui::InputText("Axiom", buf.data(), lsys_successor_size))
            {
                is_modified = true;
                lsys.set_axiom(array_to_string(buf));
            }
        }

        // The next part has a lot in common with 'interact_with(InterpretationMapBuffer, )'
        // It will be refactorized if a third similar use case come.
        { // Production rules
          // [ predecessor ] -> [ successor ] [-] (remove rule) | [+] (add rule)

            
            ImGui::Text("Production rules:");

            ImGui::Indent(); 

            // Iterator pointing to the rule to delete, if the [-] button is
            // clicked on.
            auto to_delete = buffer.end();

            // If the [+] button is clicked we add a rule to the LSystemBuffer.
            bool must_add_rule = false;

            // We use the old iterator style to save the rule to delete, if necessary.
            for (auto it = buffer.begin(); it != buffer.end(); ++it)
            { 
                auto rule = *it;

                ImGui::PushID(&(*it)); // Create a scope.
                ImGui::PushItemWidth(20);

                char predec[2] { rule.predecessor, '\0' };
                // Display the predecessor as an InputText
                if (ImGui::InputText("##pred", predec, 2))
                {
                    is_modified = true;
                    
                    // ... check if the new predecessor already exists in the rules.
                    // (does not apply to the null character)
                    buffer.delayed_change_predecessor(it, predec[0]);
                }

                ImGui::PopItemWidth(); ImGui::SameLine(); ImGui::Text("->"); ImGui::SameLine();

                ImGui::PushItemWidth(200);

                // Interact with the successor. Except for the input size, does
                // not have any constraints.
                auto array = string_to_array<lsys_successor_size>(rule.successor);
                if(ImGui::InputText("##succ", array.data(), lsys_successor_size) &&
                   predec[0] != '\0')
                {
                    is_modified = true;
                    buffer.delayed_change_successor(it, array_to_string(array));
                }
                
                
                // The [-] button. If clicked, the current iterator is saved as
                // the one to delete. We reasonably assume a user can not click
                // on two different buttons in the same frame.
                // We will need to synchronize the rules.
                ImGui::SameLine();
                if (ImGui::Button("-"))
                {
                    is_modified = true;
                    to_delete = it;
                }

                // If the current rule is not valid, add a warning.
                if(!rule.validity)
                {
                    ImGui::SameLine();
                    ImGui::TextColored(ImVec4(1.f,0.f,0.f,1.f), "Duplicated predecessor: %s", predec);
                }
                
                ImGui::PopID(); // End of the loop and the scope
            }

            ImGui::PushStyleGreenButton();
            if (ImGui::Button("+"))
            {
                is_modified = true;
                // If the button is clicked, we must add a rule to the buffer.
                must_add_rule = true;
            }
            ImGui::PopStyleColor(3);

            // Erase the marked rule if necessary
            if (to_delete != buffer.end())
            {
                buffer.delayed_erase(to_delete);
            }

            // Add a rule if necessary
            if (must_add_rule)
            {
                buffer.delayed_add_rule();
            }

            buffer.apply();
            
            ImGui::Unindent(); 
        }

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

        bool is_modified = false;

        // The next part has a lot in common with 'interact_with(LSystemBuffer, )'
        // It will be refactorized if a third similar use case come.
        
        { // Interpretations
          // [ predecessor ] -> [ interpretation ] | [-] (remove) | [+] (add)

            auto& interpretations = buffer.interpretation_buffer_;
            using validity    = InterpretationMapBuffer::validity; // == unicity
            using predecessor = InterpretationMapBuffer::predecessor;
            
            // Inform 'buffer' to synchronize the map buffer with the
            // InterpretationMap.
            bool interpretations_modified = false;

            // Iterator pointing to the rule to delete, if the [-] button is
            // clicked on.
            auto to_delete = interpretations.end();

            // If the [+] button is clicked we add a rule to the LSystemBuffer.
            bool must_add_interpretation = false;

            // We use the old iterator style to save the rule in 'to_delete', if necessary.
            for (auto it = interpretations.begin(); it != interpretations.end(); ++it)
            { 
                auto& interp = *it;
                auto& is_valid = std::get<validity>(interp);
                auto& pred = std::get<predecessor>(interp);
                auto& order = std::get<OrderEntry>(interp);

                ImGui::PushID(&interp); // Create a scope.
                ImGui::PushItemWidth(20);

                // Display the predecessor in as InputText
                if (ImGui::InputText("##pred", pred.data(), 2))
                {
                    // The predecessor has been modified by the user. Now...
                    interpretations_modified = true;
                    
                    // ... check if the new predecessor already exists in the interpretations.
                    bool is_duplicate = false;
                    for(auto find_it = interpretations.begin(); find_it != interpretations.end(); ++find_it)
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
                                                            order);
                int index = std::distance(all_orders.begin(), selected_interpretation_it);
                if(ImGui::ListBox("##order", &index, all_orders_name.data(), all_orders_name.size()))
                {
                    interpretations_modified = true;
                    order = all_orders.at(index);
                }

                // The [-] button. If clicked, the current iterator is saved as
                // the one to delete. We reasonably assume a user can not click
                // on two different buttons in the same frame.
                // We will need to synchronize the interpretations.
                ImGui::SameLine();
                if (ImGui::Button("-"))
                {
                    to_delete = it;
                    interpretations_modified = true;
                }

                // For the last interpretation in the buffer, add the [+] button.
                if (it == std::prev(interpretations.end()))
                {
                    ImGui::SameLine();
                    ImGui::PushStyleGreenButton();
                    if (ImGui::Button("+"))
                    {
                        // If the button is clicked, we must add a rule to the buffer.
                        must_add_interpretation = true;
                    }
                    ImGui::PopStyleColor(3);
                }

                // If the current interpretation is not valid, add a warning.
                if(!is_valid)
                {
                    ImGui::SameLine();
                    ImGui::TextColored(ImVec4(1.f,0.f,0.f,1.f), "Duplicated predecessor: %s", pred.data());
                }
                
                ImGui::PopID(); // End of the loop and the scope
            }

            // Erase the marked interpretation if necessary
            if (to_delete != interpretations.end())
            {
                interpretations.erase(to_delete);
            }

            // Add an interpretation if necessary
            if (must_add_interpretation)
            {
                predecessor pred;
                pred.fill('\0');
                interpretations.push_back({true, pred, go_forward_entry});
            }

            // Synchronize the interpretations if necessary
            if (interpretations_modified)
            {
                buffer.sync();
                is_modified = true;
            }
        
            
            ImGui::Unindent(); 
        }


        conclude(main);

        return is_modified;
    }
}
