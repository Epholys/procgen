#include <cmath>
#include <vector>
#include <sstream>
#include <fstream>

#include <SFML/Graphics.hpp>

#include "imgui/imgui.h"

#include "ExportMenu.h"

#include "size_computer.h"
#include "LSystemController.h"
#include "LSystemView.h"
#include "PopupGUI.h"
#include "export.h"

namespace controller
{
    namespace fs = std::filesystem;

    ExportMenu::~ExportMenu()
    {
        for (auto id : popups_ids_)
        {
            procgui::remove_popup(id);
        }
    }


    void ExportMenu::parameters()
    {
        ImGui::Text("Number of iteration for your LSystem: ");
        ImGui::SameLine();
        int iter_tmp = n_iteration_;
        if (ImGui::InputInt("##ITER", &iter_tmp, 1) && iter_tmp >= 0)
        {
            n_iteration_ = iter_tmp;
        }

        ImGui::Text("Biggest dimension of the exported image: ");
        ImGui::SameLine();
        int dim_tmp = image_dim_;
        const auto max_texture_size = sf::Texture::getMaximumSize();
        if (ImGui::InputInt("##DIM", &dim_tmp, 10) &&
            dim_tmp > 0 && dim_tmp <= int(max_texture_size))
        {
            image_dim_ = dim_tmp;
        }
        ImGui::SameLine();
        ImGui::Text("(Max size: %d)", max_texture_size);

        ImGui::Text("Ratio branch_length/branch_width: ");
        ImGui::SameLine();
        float ratio_tmp = ratio_;
        if (ImGui::InputFloat("##RATIO", &ratio_tmp, 0.05) &&
            ratio_tmp > 0.f)
        {
            ratio_ = ratio_tmp;
        }
    }

    void ExportMenu::size_warning() const
    {
        constexpr unsigned long long megabyte = 1024 * 1024;

        const auto* const lsystem = LSystemController::under_mouse();

        Expects(lsystem);

        const auto sizes = drawing::compute_max_size(*lsystem->get_lsystem_buffer().get_rule_map(),
                                                     *lsystem->get_interpretation_buffer().get_rule_map(),
                                                     n_iteration_);
        const auto total_lsys_size = drawing::memory_size(sizes);

        const auto box = lsystem->get_bounding_box();
        const auto box_max_dim = std::max(std::max(box.width, box.height), 1.f);
        const auto box_min_dim = std::max(std::min(box.width, box.height), 1.f);
        const auto scale = image_dim_ / box_max_dim;
        const auto image_size = box_max_dim * scale * box_min_dim * scale * 4; // 4 bytes per pix

        const auto total_size_mb = total_lsys_size/megabyte;
        const auto image_size_mb = static_cast<unsigned long long>(image_size)/megabyte;

        std::stringstream warning_text;
        warning_text << "You will compute a LSystem of size "
                     << (total_size_mb == 0 ? "<0" : std::to_string(total_size_mb))
                     << " MB and an image of size "
                     << (image_size_mb == 0 ? "<0" : std::to_string(image_size_mb))
                     << " MB";

        ImGui::Text(warning_text.str().c_str());
        ImGui::Text("For big L-System, the application may take a long time to compute it.");
        ImGui::Text("For bigger L-System, the application or your");
        ImGui::SameLine();
        ImGui::TextColored(ImVec4(0.8f, 0.f, 1.f, 1.f), "computer");
        ImGui::SameLine();
        ImGui::Text("will");
        ImGui::SameLine();
        ImGui::TextColored(ImVec4(0.f, 0.5f, 1.f, 1.f), "freeze");
        ImGui::SameLine();
        ImGui::Text("or");
        ImGui::SameLine();
        ImGui::TextColored(ImVec4(1.f, 0.f, 0.f, 1.f), "CRASH");
        ImGui::SameLine();
        ImGui::Text(".");

    }

    void ExportMenu::save_file()
    {
        const auto* const lsystem = LSystemController::under_mouse();
        Expects(lsystem);

        std::string save_file = save_dir_.string() + "/";
        std::string lsys_name = lsystem->get_name();
        if (lsys_name.empty())
        {
            lsys_name = "export";
        }
        save_file += lsys_name + ".png";

        ImGui::Text("The LSystem will be exported to file : ");
        ImGui::SameLine();
        ImGui::Text(save_file.c_str());

        ext::ImGui::PushStyleColoredButton<ext::ImGui::Green>();
        bool to_export = ImGui::Button("Export");
        ImGui::PopStyleColor(3);
        ImGui::SameLine();

        if (to_export)
        {
            close_menu_ = true;
            bool success = drawing::export_to_png(save_file);
            if (!success)
            {
                // Open the file error popup if the export failed.
                procgui::PopupGUI file_error_popup =
                    { "Error##FILE",
                      [save_file]()
                      {
                          std::string error_message = "Error: can't open file: "+save_file;
                          ImGui::Text(error_message.c_str());
                      }
                    };
                popups_ids_.push_back(procgui::push_popup(file_error_popup));
            }
        }
    }

    bool ExportMenu::open(sf::Keyboard::Key key)
    {
        ImGui::SetNextWindowPosCenter();
        if (ImGui::Begin("Export LSystem to PNG", NULL, ImGuiWindowFlags_NoCollapse|ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_AlwaysAutoResize))
        {
            // Text
            ImGui::TextColored(ImVec4(1.f, 0.f, 0.f, 1.f), "WARNING: ");
            ImGui::SameLine();
            ImGui::Text("This is a ");
            ImGui::SameLine();
            ImGui::TextColored(ImVec4(1.f, .5f, 0.f, 1.f), "BETA ");
            ImGui::SameLine();
            ImGui::Text("feature. It is stable, but not really polished and safe.");

            parameters();

            size_warning();

            save_file();

            ext::ImGui::PushStyleColoredButton<ext::ImGui::Red>();
            if (ImGui::Button("Cancel") || key == sf::Keyboard::Escape)
            {
                close_menu_ = true;
            }
            ImGui::PopStyleColor(3);

            ImGui::End();
        }

        if (close_menu_)
        {
            close_menu_ = false;
            return true;
        }

        return false;
    }
}
