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
        , vertices_ {}
        , bounding_box_ {}
        , sub_boxes_ {}
        , is_selected_ {false}
    {
        // Invariant respected: cohesion between the LSystem/InterpretationMap
        // and the vertices. 
        Observer<LSystem>::add_callback([this](){compute_vertices();});
        Observer<InterpretationMap>::add_callback([this](){compute_vertices();});
        compute_vertices();
    }

    LSystemView::LSystemView(const LSystemView& other)
        : Observer<LSystem> {other.Observer<LSystem>::target_}
        , Observer<InterpretationMap> {other.Observer<InterpretationMap>::target_}
        , lsys_buff_ {other.lsys_buff_}
        , interpretation_buff_ {other.interpretation_buff_}
        , params_ {other.params_}
        , vertices_ {other.vertices_}
        , bounding_box_ {other.bounding_box_}
        , sub_boxes_ {other.sub_boxes_}
        , is_selected_ {other.is_selected_}
    {
        Observer<LSystem>::add_callback([this](){compute_vertices();});
        Observer<InterpretationMap>::add_callback([this](){compute_vertices();});
    }
    LSystemView& LSystemView::operator=(const LSystemView& other)
    {
        lsys_buff_ = other.lsys_buff_;
        interpretation_buff_ = other.interpretation_buff_;
        params_ = other.params_;
        vertices_ = other.vertices_;
        bounding_box_ = other.bounding_box_;
        sub_boxes_ = other.sub_boxes_;
        is_selected_ = other.is_selected_;

        Observer<LSystem>::add_callback([this](){compute_vertices();});
        Observer<InterpretationMap>::add_callback([this](){compute_vertices();});

        return *this;
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
        // Invariant respected: cohesion between the vertices and the bounding
        // boxes. 
        
        vertices_ = drawing::compute_vertices(*Observer<LSystem>::target_,
                                              *Observer<InterpretationMap>::target_,
                                              params_);
        bounding_box_ = geometry::compute_bounding_box(vertices_);
        sub_boxes_ = geometry::compute_sub_boxes(vertices_, MAX_SUB_BOXES);
    }
    
    void LSystemView::draw(sf::RenderTarget &target)
    {
        // Interact with the models and re-compute the vertices if there is a
        // modification. 
        if (interact_with(*this, "", true, &is_selected_))
        {
            compute_vertices();
        }

        
        
        // Early out if there are no vertices.
        if (vertices_.size() == 0)
        {
            return;
        }

        // Draw the vertices.
        target.draw(vertices_.data(), vertices_.size(), sf::LineStrip);

        if (is_selected_)
        {
            // Draw the global bounding boxes.
            std::array<sf::Vertex, 5> box =
                {{ {{ bounding_box_.left, bounding_box_.top}},
                   {{ bounding_box_.left, bounding_box_.top + bounding_box_.height}},
                   {{ bounding_box_.left + bounding_box_.width, bounding_box_.top + bounding_box_.height}},
                   {{ bounding_box_.left + bounding_box_.width, bounding_box_.top}},
                   {{ bounding_box_.left, bounding_box_.top}}}};
            target.draw(box.data(), box.size(), sf::LineStrip);
        }

        // DEBUG
        // Draw the sub-bounding boxes.
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

    bool LSystemView::is_selected() const
    {
        return is_selected_;
    }

    bool LSystemView::is_inside(const sf::Vector2f& click) const
    {
        for (const auto& rect : sub_boxes_)
        {
            if (rect.contains(sf::Vector2f(click)))
            {
                return true;
            }
        }
        return false;
    }
    
    void LSystemView::select()
    {
        is_selected_ = true;
    }
}
