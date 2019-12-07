#include <cmath>
#include <vector>
#include <SFML/Graphics.hpp>
#include "imgui/imgui.h"
#include "ExportMenu.h"
#include "imgui_extension.h"

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

    bool ExportMenu::open(sf::Keyboard::Key key)
    {
        ImGui::SetNextWindowPosCenter();
        if (ImGui::Begin("Export LSystem to PNG", NULL, ImGuiWindowFlags_NoCollapse|ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_AlwaysAutoResize))
        {
            // Avoid interaction with the background when exporting a LSytem.
            ImGui::CaptureKeyboardFromApp();
            ImGui::CaptureMouseFromApp();

            // Text
            ImGui::TextColored(ImVec4(1.f, 0.f, 0.f, 1.f), "WARNING: ");
            ImGui::SameLine();
            ImGui::Text("This is a ");
            ImGui::SameLine();
            ImGui::TextColored(ImVec4(1.f, .5f, 0.f, 1.f), "BETA ");
            ImGui::SameLine();
            ImGui::Text("feature. It is stable, but not really polished and safe.");

            ImGui::Text("Number of iteration for your LSystem: ");
            ImGui::SameLine();
            static int iteration = 5;
            ImGui::InputInt("", &iteration, 1);

            ImGui::Text("Biggest dimension of the exported image: ");
            ImGui::SameLine();
            static int image_dim = 1000;
            ImGui::InputInt("", &image_dim, 10);

            ImGui::Text("Ratio branch_length/branch_width: ");
            ImGui::SameLine();
            static float ratio = 5;
            ImGui::InputFloat("", &ratio, 0.05);

            constexpr unsigned long long megabyte = 1024 * 1024;
            unsigned long long lsys_size = 10000000;
            unsigned long long image_size = 100000;
            ImGui::Text("You will compute a LSystem of size %llu MB and an image of size %llu MB",
                        lsys_size/megabyte, image_size/megabyte);
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
