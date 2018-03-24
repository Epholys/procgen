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
        , sub_boxes_ {}
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
        bounding_box_ = geometry::compute_bounding_box(vertices_);
        sub_boxes_ = geometry::compute_sub_boxes(vertices_, MAX_SUB_BOXES, MIN_VERTICES_PER_BOX);
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

        // DEBUG
        for (const auto& box : sub_boxes_)
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
