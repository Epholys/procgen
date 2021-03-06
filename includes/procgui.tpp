template<typename Buffer>
void interact_with_buffer(Buffer& buffer,
                          std::function<void(typename Buffer::const_iterator, bool&)> successor_fn)
{
    //  --- Rules ---
    // [ predecessor ] -> [ successor ] [-] (remove rule) | [+] (add rule)

    ImGui::Indent();

    // Iterator pointing to the rule to delete, if the [-] button is
    // clicked on.
    auto to_delete = buffer.end();

    bool buffer_updated = false;

    // We use the old iterator style to save the rule to delete, if necessary.
    for (auto it = buffer.begin(); it != buffer.end(); ++it)
    {
        auto rule = *it;

        ImGui::PushID(&(*it)); // Create a scope.
        ImGui::PushItemWidth(20);

        char predec[2] {rule.predecessor, '\0'};
        // Display the predecessor as an InputText
        if (ImGui::InputText("##pred", predec, 2) && !buffer_updated)
        {
            buffer.change_predecessor(it, predec[0]);
            buffer_updated = true;
        }

        ImGui::PopItemWidth();
        ImGui::SameLine();
        ImGui::Text("->");
        ImGui::SameLine();

        successor_fn(it, buffer_updated);

        // The [-] button. If clicked, the current iterator is saved as the
        // one to delete. We reasonably assume a user can not click on two
        // different buttons in the same frame.  We will need to synchronize
        // the rules.
        ImGui::PushStyleColor(ImGuiCol_Button, static_cast<ImVec4>(ImColor::HSV(0, 0.6f, 0.6f)));
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered,
                              static_cast<ImVec4>(ImColor::HSV(0, 0.7f, 0.7f)));
        ImGui::PushStyleColor(ImGuiCol_ButtonActive,
                              static_cast<ImVec4>(ImColor::HSV(0, 0.8f, 0.8f)));
        ImGui::SameLine();
        if (ImGui::Button("-") && !buffer_updated)
        {
            to_delete = it;
            buffer_updated = true;
        }
        ImGui::PopStyleColor(3);

        // If the current rule is not valid, add a warning.
        if (!rule.is_active)
        {
            ImGui::SameLine();
            ImGui::TextColored(ImVec4(1.f, 0.f, 0.f, 1.f), "Duplicated predecessor: %s", predec);
        }

        ImGui::PopID(); // End of the loop and the scope
    }

    ImGui::PushStyleColor(ImGuiCol_Button, static_cast<ImVec4>(ImColor::HSV(2 / 7.0f, 0.6f, 0.6f)));
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered,
                          static_cast<ImVec4>(ImColor::HSV(2 / 7.0f, 0.7f, 0.7f)));
    ImGui::PushStyleColor(ImGuiCol_ButtonActive,
                          static_cast<ImVec4>(ImColor::HSV(2 / 7.0f, 0.8f, 0.8f)));
    if (ImGui::Button("+") && !buffer_updated)
    {
        buffer.add_rule();
        buffer_updated = true;
    }
    ImGui::PopStyleColor(3);

    // Erase the marked rule if necessary
    if (to_delete != buffer.end())
    {
        buffer.erase(to_delete);
    }

    ImGui::Unindent();
}
