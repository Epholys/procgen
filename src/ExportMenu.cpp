#include <cmath>
#include <vector>
#include <sstream>
#include <SFML/Graphics.hpp>
#include "imgui/imgui.h"
#include "ExportMenu.h"
#include "imgui_extension.h"
#include "size_computer.h"
#include "LSystemController.h"
#include "LSystemView.h"

namespace controller
{
    std::vector<sf::Vertex> ExportMenu::add_width(const std::vector<sf::Vertex>& v, float w) const
    {
        std::vector<sf::Vertex> vw;
        vw.reserve(v.size()*2);
        auto normal = [](auto p1, auto p2){return sf::Vector2f(-(p2.y - p1.y), p2.x - p1.x);};
        auto normalize = [](auto v){auto norm=std::sqrt(v.x*v.x + v.y*v.y); if(norm) return sf::Vector2f(v.x/norm, v.y/norm); else return v;};
        for (auto i=1u; i<v.size(); ++i)
        {
            auto n = normalize(normal(v[i-1].position, v[i].position));
            auto p2 = v[i-1].position+w*n;
            auto p4 = v[i].position+w*n;
            sf::Vertex vx2 {p2, v[i-1].color};
            sf::Vertex vx4 {p4, v[i].color};
            vw.push_back(v[i-1]);
            vw.push_back(vx2);
            vw.push_back(v[i]);
            vw.push_back(vx4);
        }
        return vw;
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

        const auto* lsystem = LSystemController::under_mouse();

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

            std::string path = "saves/name.lsys.png";
            ImGui::Text("The file will be saved as ");
            ImGui::SameLine();
            ImGui::Text(path.c_str());

            ext::ImGui::PushStyleColoredButton<ext::ImGui::Green>();
            if (ImGui::Button("Export"))
            {
                export_to_png();
                close_menu_ = true;
            }
            ImGui::PopStyleColor(3);
            ImGui::SameLine();
            // ~Text



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

    void ExportMenu::export_to_png() const
    {
    }

// void tmp()
// {
//     auto box = view.get_bounding_box();
//     auto size = std::atoi(argv[2]);
//     auto s = box.width > box.height ? box.width : box.height;
//     auto ratio = size / s;

//     auto step = view.get_parameters().get_step();
//     view.ref_parameters().set_step(step*ratio);
//     box = view.get_bounding_box();
//     const auto& v = view.get_vertices();
//     std::vector<sf::Vertex> vertices;
//     if (argc > 3)
//     {
//         float width = std::atof(argv[3]);
//         vertices = add_width(v, step*ratio/width);
//     }
//     else
//         vertices = v;
//     sf::RenderTexture render;
//     render.create(std::ceil(box.width), std::ceil(box.height));
//     auto render_view = render.getView();
//     render_view.move(box.left, box.top);
//     render.setView(render_view);
//     render.clear(sf::Color::Black);
//     if (argc > 3)
//         render.draw(vertices.data(), vertices.size(), sf::TriangleStrip);
//     else
//         render.draw(vertices.data(), vertices.size(), sf::LineStrip);
//     const auto& texture = render.getTexture();
//     auto image = texture.copyToImage();
//     image.flipVertically();
//     image.saveToFile(std::string(argv[1])+".png");
// }
}
