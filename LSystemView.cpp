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
        compute_bounding_box();
    }

    void LSystemView::compute_bounding_box()
    {
        if (vertices_.size() == 0)
        {
            return;
        }
        const auto& first = vertices_.at(0);
        // warning: top is at low value.
        float top = first.position.y, down = first.position.y;
        float left = first.position.x, right = first.position.x;
        for (const auto& v : vertices_)
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
        bounding_box_ = {left, top, right - left, down - top};
    }
    
    void LSystemView::draw(sf::RenderTarget &target)
    {
        if (interact_with(*this, ""))
        {
            compute_vertices();
        };
        target.draw(vertices_.data(), vertices_.size(), sf::LineStrip);

        std::array<sf::Vertex, 5> box =
            {{ {{ bounding_box_.left, bounding_box_.top}},
               {{ bounding_box_.left, bounding_box_.top + bounding_box_.height}},
               {{ bounding_box_.left + bounding_box_.width, bounding_box_.top + bounding_box_.height}},
               {{ bounding_box_.left + bounding_box_.width, bounding_box_.top}},
               {{ bounding_box_.left, bounding_box_.top}}}};
        target.draw(box.data(), box.size(), sf::LineStrip);
    }
}
