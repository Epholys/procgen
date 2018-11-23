#include "procgui.h"
#include "LSystemView.h"
#include "helper_math.h"

namespace procgui
{
    using namespace drawing;
    using namespace colors;

    int LSystemView::id_count_ = 0;
    UniqueColor LSystemView::color_gen_ {};
    
    LSystemView::LSystemView(const std::string& name,
                             std::shared_ptr<LSystem> lsys,
                             std::shared_ptr<InterpretationMap> map,
                             std::shared_ptr<DrawingParameters> params,
                             std::shared_ptr<VertexPainter> painter)
        : OLSys {lsys}
        , OMap {map}
        , OParams {params}
        , OPainter {painter}
        , id_{id_count_++}
        , color_id_{color_gen_.register_id(id_)}
        , name_ {name}
        , lsys_buff_ {lsys}
        , interpretation_buff_ {map}
        , vertices_ {}
        , bounding_box_ {}
        , sub_boxes_ {}
        , is_selected_ {false}
    {
        // Invariant respected: cohesion between the LSystem/InterpretationMap
        // and the vertices. 
        OLSys::add_callback([this](){compute_vertices();});
        OMap::add_callback([this](){compute_vertices();});
        OPainter::add_callback([this](){paint_vertices();});
        OParams::add_callback([this](){compute_vertices();});
            
        compute_vertices();
    }

    LSystemView::LSystemView(const sf::Vector2f& position)
        : LSystemView(
            "",
            std::make_shared<LSystem>(LSystem("F+F+F+F", {})),
            std::make_shared<InterpretationMap>(default_interpretation_map),
            std::make_shared<DrawingParameters>(position))
    {
        // Arbitrary default LSystem.
    }

    LSystemView::LSystemView(const LSystemView& other)
        : OLSys {other.OLSys::get_target()}
        , OMap {other.OMap::get_target()}
        , OParams {other.OParams::get_target()}
        , OPainter {other.OPainter::get_target()}
        , id_ {id_count_++}
        , color_id_{color_gen_.register_id(id_)}
        , name_ {other.name_}
        , lsys_buff_ {other.lsys_buff_}
        , interpretation_buff_ {other.interpretation_buff_}
        , vertices_ {other.vertices_}
        , bounding_box_ {other.bounding_box_}
        , sub_boxes_ {other.sub_boxes_}
        , is_selected_ {other.is_selected_}
    {
        // Manually managing Observer<> callbacks.
        OLSys::add_callback([this](){compute_vertices();});
        OMap::add_callback([this](){compute_vertices();});
        OParams::add_callback([this](){compute_vertices();});
        OPainter::add_callback([this](){paint_vertices();});
    }

    LSystemView::LSystemView(LSystemView&& other)
        : OLSys {other.OLSys::get_target()}
        , OMap {other.OMap::get_target()}
        , OParams {other.OParams::get_target()}
        , OPainter {other.OPainter::get_target()}
        , id_ {other.id_}
        , color_id_{std::move(other.color_id_)}
        , name_ {std::move(other.name_)}
        , lsys_buff_ {std::move(other.lsys_buff_)}
        , interpretation_buff_ {std::move(other.interpretation_buff_)}
        , vertices_ {std::move(other.vertices_)}
        , bounding_box_ {std::move(other.bounding_box_)}
        , sub_boxes_ {std::move(other.sub_boxes_)}
        , is_selected_ {other.is_selected_}
    {
        // Manually managing Observer<> callbacks.
        OLSys::add_callback([this](){compute_vertices();});
        OMap::add_callback([this](){compute_vertices();});
        OParams::add_callback([this](){compute_vertices();});
        OPainter::add_callback([this](){paint_vertices();});

        // Remove callbacks of the moved 'other'.
        other.OLSys::set_target(nullptr);
        other.OMap::set_target(nullptr);
        other.OParams::set_target(nullptr);
        other.OPainter::set_target(nullptr);

        // the 'other' object must not matter in the 'color_gen_' anymore.
        other.id_ = -1;
        other.color_id_ = sf::Color::Black;
        other.bounding_box_ = {};
        other.is_selected_ = false;
    }

    LSystemView& LSystemView::operator=(LSystemView other)
    {
        swap(other);
        return *this;
    }

    void LSystemView::swap(LSystemView& other)
    {
        using std::swap;

        // Not a pure swap but Observer<> must be manually swaped:
        //  - swap the targets.
        //  - add correct callback.
        auto tmp_lsys = OLSys::get_target();
        OLSys::set_target(other.OLSys::get_target());
        other.OLSys::set_target(tmp_lsys);
        OLSys::add_callback([this](){compute_vertices();});
        other.OLSys::add_callback([&other](){other.compute_vertices();});

        auto tmp_map = OMap::get_target();
        OMap::set_target(other.OMap::get_target());
        other.OMap::set_target(tmp_map);
        OMap::add_callback([this](){compute_vertices();});
        other.OMap::add_callback([&other](){other.compute_vertices();});

        auto tmp_params = OParams::get_target();
        OParams::set_target(other.OParams::get_target());
        other.OParams::set_target(tmp_params);
        OParams::add_callback([this](){compute_vertices();});
        other.OParams::add_callback([&other](){other.compute_vertices();});

        auto tmp_painter = OPainter::get_target();
        OPainter::set_target(other.OPainter::get_target());
        other.OPainter::set_target(tmp_painter);
        OPainter::add_callback([this](){paint_vertices();});
        other.OPainter::add_callback([&other](){other.paint_vertices();});

        swap(id_, other.id_);
        swap(color_id_, other.color_id_);
        swap(name_, other.name_);
        swap(lsys_buff_, other.lsys_buff_);
        swap(interpretation_buff_, other.interpretation_buff_);
        swap(vertices_, other.vertices_);
        swap(bounding_box_, other.bounding_box_);
        swap(sub_boxes_, other.sub_boxes_);
        swap(is_selected_, other.is_selected_);
    }

    LSystemView::~LSystemView()
    {
        // Unregister the id unless the object was moved.
        if (id_ != -1)
        {
            color_gen_.remove_id(id_);
        }
    }


    LSystemView LSystemView::clone() const
    {        
        // Deep copy.
        return LSystemView(
            name_,
            std::make_shared<LSystem>(*OLSys::get_target()),
            std::make_shared<InterpretationMap>(*OMap::get_target()),
            std::make_shared<DrawingParameters>(*OParams::get_target())
            );
    }

    LSystemView LSystemView::duplicate() const
    {
        return LSystemView(
            name_,
            lsys_buff_.get_target(),
            interpretation_buff_.get_target(),
            std::make_shared<DrawingParameters>(*OParams::get_target()),
            OPainter::get_target());
    }
    

    drawing::DrawingParameters& LSystemView::ref_parameters()
    {
        return *OParams::get_target();
    }
    LSystemBuffer& LSystemView::ref_lsystem_buffer()
    {
        return lsys_buff_;
    }
    InterpretationMapBuffer& LSystemView::ref_interpretation_buffer()
    {
        return interpretation_buff_;
    }
    colors::VertexPainter& LSystemView::ref_vertex_painter()
    {
        return *OPainter::get_target();
    }
    sf::FloatRect LSystemView::get_bounding_box() const
    {
        return get_transform().transformRect(bounding_box_);
    }
    const drawing::DrawingParameters& LSystemView::get_parameters() const
    {
        return *OParams::get_target();
    }
    const LSystemBuffer& LSystemView::get_lsystem_buffer() const
    {
        return lsys_buff_;
    }
    const InterpretationMapBuffer& LSystemView::get_interpretation_buffer() const
    {
        return interpretation_buff_;
    }
    const colors::VertexPainter& LSystemView::get_vertex_painter()
    {
        return *OPainter::get_target();
    }
    int LSystemView::get_id() const
    {
        return id_;
    }
    sf::Color LSystemView::get_color() const
    {
        return color_id_;
    }
    sf::Transform LSystemView::get_transform() const
    {
        sf::Transform transform;
        transform.translate(OParams::get_target()->get_starting_position());
        return transform;
    }
    
    void LSystemView::compute_vertices()
    {
        // Invariant respected: cohesion between the vertices and the bounding
        // boxes. 
        
        vertices_ = drawing::compute_vertices(*OLSys::get_target(),
                                              *OMap::get_target(),
                                              *OParams::get_target());
        //params_);
        bounding_box_ = geometry::bounding_box(vertices_);
        sub_boxes_ = geometry::sub_boxes(vertices_, MAX_SUB_BOXES);
        paint_vertices();
    }

    void LSystemView::paint_vertices()
    {
        // un-transformed vertices and bounding box
        OPainter::get_target()->paint_vertices(vertices_, bounding_box_);
    }

    
    void LSystemView::draw(sf::RenderTarget &target)
    {
        // Interact with the models.
        interact_with(*this, name_, &is_selected_);

        // Early out if there are no vertices.
        if (vertices_.size() == 0)
        {
            return;
        }

        // Draw the vertices.
        target.draw(vertices_.data(), vertices_.size(), sf::LineStrip, get_transform());

        if (is_selected_)
        {
            auto box = get_transform().transformRect(bounding_box_);
            // Draw the global bounding boxes with the unique color.
            std::array<sf::Vertex, 5> rect =
                {{ {{ box.left, box.top}, color_id_},
                   {{ box.left, box.top + box.height}, color_id_},
                   {{ box.left + box.width, box.top + box.height}, color_id_},
                   {{ box.left + box.width, box.top}, color_id_},
                   {{ box.left, box.top}, color_id_}}};
            target.draw(rect.data(), rect.size(), sf::LineStrip);
        }

        // // DEBUG
        // // Draw the sub-bounding boxes.
        // for (const auto& box : sub_boxes_)
        // {
        //     std::array<sf::Vertex, 5> rect =
        //         {{ {{ box.left, box.top}, sf::Color(255,0,0,50)},
        //            {{ box.left, box.top + box.height}, sf::Color(255,0,0,50)},
        //            {{ box.left + box.width, box.top + box.height}, sf::Color(255,0,0,50)},
        //            {{ box.left + box.width, box.top}, sf::Color(255,0,0,50)}}};
        //     target.draw(rect.data(), rect.size(), sf::Quads);
        // }
    }

    bool LSystemView::is_selected() const
    {
        return is_selected_;
    }

    bool LSystemView::is_inside(const sf::Vector2f& click) const
    {
        decltype(sub_boxes_) subs;
        for (const auto& box : sub_boxes_)
        {
            subs.push_back(get_transform().transformRect(box));
        }
        for (const auto& rect : subs)
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
