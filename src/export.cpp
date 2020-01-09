#include "export.h"

#include "LSystemView.h"

namespace drawing
{
std::vector<sf::Vertex> add_width(const std::vector<sf::Vertex>& v, float w)
{
    std::vector<sf::Vertex> vw;
    vw.reserve(v.size() * 2);
    auto normal = [](auto p1, auto p2) { return sf::Vector2f(-(p2.y - p1.y), p2.x - p1.x); };
    auto normalize = [](auto v) {
        auto norm = std::sqrt(v.x * v.x + v.y * v.y);
        if (norm)
        {
            return sf::Vector2f(v.x / norm, v.y / norm);
        }
        {
            return v;
        }
    };
    for (auto i = 1u; i < v.size(); ++i)
    {
        auto n = normalize(normal(v[i - 1].position, v[i].position));
        auto p2 = v[i - 1].position + w * n;
        auto p4 = v[i].position + w * n;
        sf::Vertex vx2 {p2, v[i - 1].color};
        sf::Vertex vx4 {p4, v[i].color};
        vw.push_back(v[i - 1]);
        vw.push_back(vx2);
        vw.push_back(v[i]);
        vw.push_back(vx4);
    }
    return vw;
}


bool export_to_png(procgui::LSystemView view,
                   const std::string& filename,
                   int n_iter,
                   int image_dim,
                   double ratio)
{
    view.set_headless(true);
    view.ref_parameters().set_n_iter(n_iter);

    auto box = view.get_bounding_box();
    auto max_dim = std::max(box.width, box.height);
    auto dim_ratio = image_dim / max_dim;

    if (max_dim == 0)
    {
        return false;
    }

    auto step = view.get_parameters().get_step();
    view.ref_parameters().set_step(step * dim_ratio);
    box = view.get_bounding_box();
    const auto& v = view.get_turtle().vertices_;

    std::vector<sf::Vertex> vertices = add_width(v, 1 / ratio);

    sf::RenderTexture render;
    render.create(std::ceil(box.width), std::ceil(box.height));
    auto render_view = render.getView();
    render_view.move(box.left, box.top);
    render.setView(render_view);
    render.clear(sf::Color::Black);

    render.draw(vertices.data(), vertices.size(), sf::TriangleStrip, view.get_transform());

    const auto& texture = render.getTexture();
    auto image = texture.copyToImage();
    image.flipVertically();
    return image.saveToFile(filename);
}

} // namespace drawing
