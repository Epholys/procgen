#include "procgui.h"
#include "LSystemView.h"

namespace procgui
{
    using namespace drawing;
    
    LSystemView::LSystemView(std::shared_ptr<LSystem> lsys,
                             std::shared_ptr<drawing::InterpretationMap> map,
                             drawing::DrawingParameters params)
        : Observer<LSystem> {lsys}
        , Observer<InterpretationMap> {map}
        , lsys_buff_ {lsys}
        , interpretation_buff_ {map}
        , params_ {params}
        , bounding_box_ {}
        , collision_boxes_ {}
    {
        Observer<LSystem>::add_callback([this](){compute_vertices();});
        Observer<InterpretationMap>::add_callback([this](){compute_vertices();});
        compute_vertices();

    }

    drawing::DrawingParameters& LSystemView::get_parameters()
    {
        return params_;
    }
    LSystemBuffer& LSystemView::get_lsystem_buffer()
    {
        return lsys_buff_;
    }
    InterpretationMapBuffer& LSystemView::get_interpretation_buffer()
    {
        return interpretation_buff_;
    }

    
    void LSystemView::compute_vertices()
    {
        vertices_ = drawing::compute_vertices(lsys_buff_.get_target(),
                                              interpretation_buff_.get_target(),
                                              params_);
        bounding_box_ = compute_bounding_box(vertices_);
        compute_collision_boxes();
    }

    sf::FloatRect LSystemView::compute_bounding_box(const std::vector<sf::Vertex>& vertices) const
    {
        if (vertices.size() == 0)
        {
            return { -1, -1, -1, -1 };
        }
        const auto& first = vertices.at(0);
        // warning: top is at low value.
        float top = first.position.y, down = first.position.y;
        float left = first.position.x, right = first.position.x;
        for (const auto& v : vertices)
        {
            if (v.position.y < top)
            {
                top = v.position.y;
            }
            else if (v.position.y > down)
            {
                down = v.position.y;
            }

            if (v.position.x > right)
            {
                right = v.position.x;
            }
            else if (v.position.x < left)
            {
                left = v.position.x;
            }
        }
        return {left, top, right - left, down - top};
    }

    void LSystemView::compute_collision_boxes()
    {
        collision_boxes_.clear();
        int vertices_per_box = vertices_.size() / N_COLLISION_BOXES;
        vertices_per_box = vertices_per_box != 0 ? vertices_per_box : 1;

        int n = 0;
        std::vector<sf::Vertex> box_vertices;
        for (size_t i = 0; i<vertices_.size(); ++i)
        {
            ++n;
            box_vertices.push_back(vertices_.at(i));
            if (vertices_.at(i).color == sf::Color::Transparent ||
                n == vertices_per_box ||
                i == vertices_.size() - 1)
            {
                n = 0;
                collision_boxes_.push_back(compute_bounding_box(box_vertices));
                box_vertices.clear();
            }
        }
    }
    
    void LSystemView::draw(sf::RenderTarget &target)
    {
        if (interact_with(*this, ""))
        {
            compute_vertices();
        }

        if (vertices_.size() == 0)
        {
            return;
        }
        
        target.draw(vertices_.data(), vertices_.size(), sf::LineStrip);

        std::array<sf::Vertex, 5> box =
            {{ {{ bounding_box_.left, bounding_box_.top}},
               {{ bounding_box_.left, bounding_box_.top + bounding_box_.height}},
               {{ bounding_box_.left + bounding_box_.width, bounding_box_.top + bounding_box_.height}},
               {{ bounding_box_.left + bounding_box_.width, bounding_box_.top}},
               {{ bounding_box_.left, bounding_box_.top}}}};
        target.draw(box.data(), box.size(), sf::LineStrip);

        for (const auto& box : collision_boxes_)
        {
            std::array<sf::Vertex, 5> rect =
                {{ {{ box.left, box.top}, sf::Color(255,0,0,50)},
                   {{ box.left, box.top + box.height}, sf::Color(255,0,0,50)},
                   {{ box.left + box.width, box.top + box.height}, sf::Color(255,0,0,50)},
                   {{ box.left + box.width, box.top}, sf::Color(255,0,0,50)}}};
            target.draw(rect.data(), rect.size(), sf::Quads);
        }
    }
}
