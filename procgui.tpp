template<typename Buffer>
bool interact_with_buffer(Buffer& buffer,
                          std::function<bool(typename Buffer::const_iterator)>  predecessor_fn)
{
    // ImGui::PushID(&buffer);

    //  --- Rules ---
    // [ predecessor ] -> [ successor ] [-] (remove rule) | [+] (add rule)

    // Returned value informing if the object was modified
    bool is_modified = false;

    ImGui::Indent(); 

    // Iterator pointing to the rule to delete, if the [-] button is
    // clicked on.
    auto to_delete = buffer.end();

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
            buffer.delayed_change_predecessor(it, predec[0]);
        }

        ImGui::PopItemWidth(); ImGui::SameLine(); ImGui::Text("->"); ImGui::SameLine();

        ImGui::PushItemWidth(200);

        is_modified |= predecessor_fn(it);
        
        // The [-] button. If clicked, the current iterator is saved as the
        // one to delete. We reasonably assume a user can not click on two
        // different buttons in the same frame.  We will need to synchronize
        // the rules.
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

    ImGui::PushStyleColor(ImGuiCol_Button, ImColor::HSV(2/7.0f, 0.6f, 0.6f));
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImColor::HSV(2/7.0f, 0.7f, 0.7f));
    ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImColor::HSV(2/7.0f, 0.8f, 0.8f));
    if (ImGui::Button("+"))
    {
        is_modified = true;
        buffer.delayed_add_rule();
    }
    ImGui::PopStyleColor(3);

    // Erase the marked rule if necessary
    if (to_delete != buffer.end())
    {
        buffer.delayed_erase(to_delete);
    }

    // Apply the buffered methods.
    buffer.apply();
            
    ImGui::Unindent();

    // ImGui::PopID();

    return is_modified;
}
