#include "procgui.h"
#include "LSystemView.h"
#include "helper_math.h"

namespace procgui
{
    using namespace drawing;
    using namespace colors;

    // int LSystemView::id_count_ = 0;
    UniqueId LSystemView::unique_ids_ {};
    UniqueColor LSystemView::unique_colors_ {};

    void LSystemView::update_callbacks()
    {
        OLSys::add_callback([this](){compute_vertices();});
        OMap::add_callback([this](){compute_vertices();});
        OParams::add_callback([this](){compute_vertices();});
        OPainter::add_callback([this](){paint_vertices();});
    }

    LSystemView::LSystemView(const std::string& name,
                             std::shared_ptr<LSystem> lsys,
                             std::shared_ptr<InterpretationMap> map,
                             std::shared_ptr<DrawingParameters> params,
                             std::shared_ptr<VertexPainterWrapper> painter)
        : OLSys {lsys}
        , OMap {map}
        , OParams {params}
        , OPainter {painter}
        , id_{unique_ids_.get_id()}
        , color_id_{unique_colors_.get_color(id_)}
        , name_ {name}
        , lsys_buff_ {lsys}
        , interpretation_buff_ {map}
        , vertices_ {}
        , iteration_of_vertices_ {}
        , max_iteration_ {0}
        , bounding_box_ {}
        , sub_boxes_ {}
        , is_selected_ {false}
    {
        // Invariant respected: cohesion between the LSystem/InterpretationMap
        // and the vertices.             
        update_callbacks();
        
        compute_vertices();
        paint_vertices();
    }

    LSystemView::LSystemView(const ext::sf::Vector2d& position)
        : LSystemView(
            "",
            std::make_shared<LSystem>(LSystem("F+F+F+F", {}, "")),
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
        , id_{unique_ids_.get_id()}
        , color_id_{unique_colors_.get_color(id_)}
        , name_ {other.name_}
        , lsys_buff_ {other.lsys_buff_}
        , interpretation_buff_ {other.interpretation_buff_}
        , vertices_ {other.vertices_}
        , iteration_of_vertices_ {other.iteration_of_vertices_}
        , max_iteration_ {other.max_iteration_}
        , bounding_box_ {other.bounding_box_}
        , sub_boxes_ {other.sub_boxes_}
        , is_selected_ {other.is_selected_}
    {
        // Manually managing Observer<> callbacks.
        update_callbacks();
    }

    LSystemView::LSystemView(LSystemView&& other)
        : OLSys {std::move(other.OLSys::get_target())}
        , OMap {std::move(other.OMap::get_target())}
        , OParams {std::move(other.OParams::get_target())}
        , OPainter {std::move(other.OPainter::get_target())}
        , id_ {other.id_}
        , color_id_{std::move(other.color_id_)}
        , name_ {std::move(other.name_)}
        , lsys_buff_ {std::move(other.lsys_buff_)}
        , interpretation_buff_ {std::move(other.interpretation_buff_)}
        , vertices_ {std::move(other.vertices_)}
        , iteration_of_vertices_ {std::move(other.iteration_of_vertices_)}
        , max_iteration_ {other.max_iteration_}
        , bounding_box_ {std::move(other.bounding_box_)}
        , sub_boxes_ {std::move(other.sub_boxes_)}
        , is_selected_ {other.is_selected_}
    {
        // Manually managing Observer<> callbacks.
        update_callbacks();

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

    LSystemView& LSystemView::operator=(const LSystemView& other)
    {
        if (this != &other)
        {
            OLSys::set_target(other.OLSys::get_target());
            OMap::set_target(other.OMap::get_target());
            OParams::set_target(other.OParams::get_target());
            OPainter::set_target(other.OPainter::get_target());
            id_ = unique_ids_.get_id();
            color_id_ = unique_colors_.get_color(id_);
            name_ = {other.name_};
            lsys_buff_ = {other.lsys_buff_};
            interpretation_buff_ = {other.interpretation_buff_};
            vertices_ = {other.vertices_};
            iteration_of_vertices_ = {other.iteration_of_vertices_};
            max_iteration_ = {other.max_iteration_};
            bounding_box_ = {other.bounding_box_};
            sub_boxes_ = {other.sub_boxes_};
            is_selected_ = {other.is_selected_};

            update_callbacks();
        }

        return *this;
    }

    LSystemView& LSystemView::operator=(LSystemView&& other)
    {
        if (this != &other)
        {
            OLSys::set_target(std::move(other.OLSys::get_target()));
            OMap::set_target(std::move(other.OMap::get_target()));
            OParams::set_target(std::move(other.OParams::get_target()));
            OPainter::set_target(std::move(other.OPainter::get_target()));
            id_ = other.id_;
            color_id_ = other.color_id_;
            name_ = {std::move(other.name_)};
            lsys_buff_ = {std::move(other.lsys_buff_)};
            interpretation_buff_ = {std::move(other.interpretation_buff_)};
            vertices_ = {std::move(other.vertices_)};
            iteration_of_vertices_ = {std::move(other.iteration_of_vertices_)};
            max_iteration_ = {other.max_iteration_};
            bounding_box_ = {std::move(other.bounding_box_)};
            sub_boxes_ = {std::move(other.sub_boxes_)};
            is_selected_ = {other.is_selected_};

            // Manually managing Observer<> callbacks.
            update_callbacks();

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

        return *this;
    }


    LSystemView::~LSystemView()
    {
        // Unregister the id unless the object was moved.
        if (id_ != -1)
        {
            unique_ids_.free_id(id_);
        }
    }


    LSystemView LSystemView::clone() const
    {        
        // Deep copy.
        return LSystemView(
            name_,
            std::make_shared<LSystem>(*OLSys::get_target()),
            std::make_shared<InterpretationMap>(*OMap::get_target()),
            std::make_shared<DrawingParameters>(*OParams::get_target()),
            std::make_shared<VertexPainterWrapper>(OPainter::get_target()->clone())
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
    colors::VertexPainterWrapper& LSystemView::ref_vertex_painter_wrapper()
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
    const colors::VertexPainterWrapper& LSystemView::get_vertex_painter_wrapper() const
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
        transform.translate(sf::Vector2f(OParams::get_target()->get_starting_position()));
        return transform;
    }
    
    void LSystemView::compute_vertices()
    {
        // Invariant respected: cohesion between the vertices and the bounding
        // boxes. 
        
        std::tie(vertices_, iteration_of_vertices_, max_iteration_) =
            drawing::compute_vertices(*OLSys::get_target(),
                                      *OMap::get_target(),
                                      *OParams::get_target());
        bounding_box_ = geometry::bounding_box(vertices_);
        sub_boxes_ = geometry::sub_boxes(vertices_, MAX_SUB_BOXES);
        geometry::expand_boxes(sub_boxes_);
        paint_vertices();
    }

    void LSystemView::paint_vertices()
    {
        // un-transformed vertices and bounding box
        OPainter::get_target()->get_target()->paint_vertices(vertices_,
                                                             iteration_of_vertices_,
                                                             max_iteration_,
                                                             bounding_box_);
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
        //     target.draw(rect.data(), rect.size(), sf::Quads, get_transform());
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
