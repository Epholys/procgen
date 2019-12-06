#include "LSystemView.h"
#include "procgui.h"
#include "helper_color.h"
#include "helper_math.h"
#include "WindowController.h"
#include "RenderWindow.h"
#include "SupplementaryRendering.h"
#include "PopupGUI.h"
#include "config.h"

namespace procgui
{
    using namespace drawing;
    using namespace colors;
    using namespace procgui;

    // int LSystemView::id_count_ = 0;
    UniqueId LSystemView::unique_ids_ {};
    UniqueColor LSystemView::unique_colors_ {};

    void LSystemView::update_callbacks()
    {
        OLSys::add_callback([this](){size_safeguard();});
        OMap::add_callback([this](){size_safeguard();});
        OParams::add_callback([this](){size_safeguard();});
        OPainter::add_callback([this](){paint_vertices();});
    }

    LSystemView::LSystemView(const std::string& name,
                             std::shared_ptr<LSystem> lsys,
                             std::shared_ptr<InterpretationMap> map,
                             std::shared_ptr<DrawingParameters> params,
                             std::shared_ptr<VertexPainterWrapper> painter)
        : OLSys (std::make_shared<LSystemBuffer>(lsys))
        , OMap (std::make_shared<InterpretationMapBuffer>(map))
        , OParams {params}
        , OPainter {painter}
        , id_{unique_ids_.get_id()}
        , color_id_{unique_colors_.get_color(id_)}
        , name_ {name}
        , is_modified_{false}
        , turtle_{*params}
        , max_iteration_ {0}
        , bounding_box_ {}
        , sub_boxes_ {}
        , is_selected_ {false}
        , bounding_box_is_visible_{true}
        , popups_ids_{}
    {
        // Invariant respected: cohesion between the LSystem/InterpretationMap
        // and the vertices.
        update_callbacks();

        size_safeguard();
    }

    LSystemView::LSystemView(const ext::sf::Vector2d& position, double step)
        : LSystemView(
            "",
            std::make_shared<LSystem>(LSystem("F+F+F+F", {}, "")),
            std::make_shared<InterpretationMap>(default_interpretation_map),
            std::make_shared<DrawingParameters>(position, step))
    {
    }

    LSystemView::LSystemView(const LSystemView& other)
        : OLSys {std::make_shared<LSystemBuffer>(
            std::make_shared<LSystem>(*other.OLSys::get_target()->get_rule_map()))}
        , OMap {std::make_shared<InterpretationMapBuffer>(
            std::make_shared<InterpretationMap>(*other.OMap::get_target()->get_rule_map()))}
        , OParams {std::make_shared<DrawingParameters>(*other.OParams::get_target())}
        , OPainter {std::make_shared<VertexPainterWrapper>(*other.OPainter::get_target())}
        , id_{unique_ids_.get_id()}
        , color_id_{unique_colors_.get_color(id_)}
        , name_ {other.name_}
        , is_modified_{other.is_modified_}
        , turtle_{other.turtle_}
        , max_iteration_ {other.max_iteration_}
        , bounding_box_ {other.bounding_box_}
        , sub_boxes_ {other.sub_boxes_}
        , is_selected_ {false}
        , bounding_box_is_visible_{true}
        , max_mem_size_{other.max_mem_size_}
        , popups_ids_{}
    {
        // Manually managing Observer<> callbacks.
        update_callbacks();

        size_safeguard();
    }

    LSystemView::LSystemView(LSystemView&& other)
        : OLSys {other.OLSys::get_target()} // with std::move, clang warning
        , OMap {other.OMap::get_target()}
        , OParams {other.OParams::get_target()}
        , OPainter {other.OPainter::get_target()}
        , id_ {other.id_}
        , color_id_{std::move(other.color_id_)}
        , name_ {std::move(other.name_)}
        , is_modified_{other.is_modified_}
        , turtle_{other.turtle_}
        , max_iteration_ {other.max_iteration_}
        , bounding_box_ {std::move(other.bounding_box_)}
        , sub_boxes_ {std::move(other.sub_boxes_)}
        , is_selected_ {false}
        , bounding_box_is_visible_{true}
        , max_mem_size_{other.max_mem_size_}
        , popups_ids_{}
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
        other.is_modified_ = false;

        for (int id : other.popups_ids_)
        {
            procgui::remove_popup(id);
        }
    }

    LSystemView& LSystemView::operator=(const LSystemView& other)
    {
        if (this != &other)
        {
            OLSys::set_target(
                std::make_shared<LSystemBuffer>(
                    std::make_shared<LSystem>(*other.OLSys::get_target()->get_rule_map())));
            OMap::set_target(
                std::make_shared<InterpretationMapBuffer>(
                    std::make_shared<InterpretationMap>(*other.OMap::get_target()->get_rule_map())));
            OParams::set_target(std::make_shared<DrawingParameters>(*other.OParams::get_target()));
            OPainter::set_target(std::make_shared<VertexPainterWrapper>(*other.OPainter::get_target()));
            id_ = unique_ids_.get_id();
            color_id_ = unique_colors_.get_color(id_);
            name_ = other.name_;
            is_modified_ = other.is_modified_;
            turtle_ = other.turtle_;
            max_iteration_ = other.max_iteration_;
            bounding_box_ = other.bounding_box_;
            sub_boxes_ = other.sub_boxes_;
            is_selected_ = false;
            bounding_box_is_visible_ = true;
            max_mem_size_ = other.max_mem_size_;
            popups_ids_ = {};

            update_callbacks();

            size_safeguard();
        }

        return *this;
    }

    LSystemView& LSystemView::operator=(LSystemView&& other)
    {
        if (this != &other)
        {
            OLSys::set_target(other.OLSys::get_target());
            OMap::set_target(other.OMap::get_target());
            OParams::set_target(other.OParams::get_target());
            OPainter::set_target(other.OPainter::get_target());
            id_ = other.id_;
            color_id_ = other.color_id_;
            name_ = std::move(other.name_);
            is_modified_ = other.is_modified_;
            turtle_ = other.turtle_;
            max_iteration_ = other.max_iteration_;
            bounding_box_ = std::move(other.bounding_box_);
            sub_boxes_ = std::move(other.sub_boxes_);
            is_selected_ = false;
            bounding_box_is_visible_ = true;
            max_mem_size_ =other.max_mem_size_;
            popups_ids_ = {};

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
            other.is_modified_ = false;
            for (int id : other.popups_ids_)
            {
                procgui::remove_popup(id);
            }
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

        for (auto id : popups_ids_)
        {
            procgui::remove_popup(id);
        }
    }

    DrawingParameters& LSystemView::ref_parameters()
    {
        return *OParams::get_target();
    }
    LSystemBuffer& LSystemView::ref_lsystem_buffer()
    {
        return *OLSys::get_target();
    }
    InterpretationMapBuffer& LSystemView::ref_interpretation_buffer()
    {
        return *OMap::get_target();
    }
    VertexPainterWrapper& LSystemView::ref_vertex_painter_wrapper()
    {
        return *OPainter::get_target();
    }
    sf::FloatRect LSystemView::get_bounding_box() const
    {
        return get_transform().transformRect(bounding_box_);
    }
    const DrawingParameters& LSystemView::get_parameters() const
    {
        return *OParams::get_target();
    }
    const LSystemBuffer& LSystemView::get_lsystem_buffer() const
    {
        return *OLSys::get_target();
    }
    const InterpretationMapBuffer& LSystemView::get_interpretation_buffer() const
    {
        return *OMap::get_target();
    }
    const VertexPainterWrapper& LSystemView::get_vertex_painter_wrapper() const
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
        const auto scale_factor = OParams::get_target()->get_step() / Turtle::step_;
        transform.scale(scale_factor, scale_factor);
        return transform;
    }
    std::string LSystemView::get_name() const
    {
        return name_;
    }
    void LSystemView::set_name(const std::string& name)
    {
        name_ = name;
        is_modified_ = false;
    }

    bool LSystemView::is_modified() const
    {
        return is_modified_;
    }
    void LSystemView::finish_loading()
    {
        is_modified_ = false;
    }


    void LSystemView::size_safeguard()
    {
        drawing::system_size size = compute_max_size(*OLSys::get_target()->get_rule_map(),
                                                     *OMap::get_target()->get_rule_map(),
                                                      OParams::get_target()->get_n_iter());
        system_size_ = size;
        auto approximate_mem_size_ = drawing::memory_size(size);
        auto max_size = std::max(max_mem_size_, config::sys_max_size);

        if (approximate_mem_size_ > max_size)
        {
            open_size_warning_popup();
        }
        else
        {
            // Validate all changes.
            OParams::get_target()->validate();
            OLSys::get_target()->validate();
            OMap::get_target()->validate();

            compute_vertices();
        }
    }
    void LSystemView::open_size_warning_popup()
    {
        procgui::PopupGUI size_warning_popup =
            { "Size Warning##LSysView",
              [this]()
              {
                  constexpr drawing::Matrix::number megabyte = 1024 * 1024;
                  const auto approximate_mem_size = drawing::memory_size(system_size_);
                  ImGui::TextColored(ImVec4(1.f, 0.5f, 0.f, 1.f), "WARNING\n");
                  if (approximate_mem_size == drawing::Matrix::MAX)
                  {
                      ImGui::Text("You are trying to compute a big L-System of a size bigger than 16 exabytes");
                      ImGui::Text("(bigger than the data stored by Google in 2013).");
                      ImGui::Text("You still have the choice to proceed if you have alien tech,");
                      ImGui::Text("but otherwise the application or your");
                  }
                  else
                  {
                      ImGui::Text("You are trying to compute a big L-System of size %llu MB, do you want to continue?", approximate_mem_size/megabyte);
                      ImGui::Text("For big L-System, the application may take a long time to compute it.");
                      ImGui::Text("For bigger L-System, the application or your");
                  }
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

                  ImGui::Text("You can change the global size limit in the \"Application parameters\" section of any L-System");

                  ImGui::Text("Selecting 'OK' will start the computation, this popup will stay open during this time.");
              },
              false, "Yes", "No",
              [this]()
              {
                  // We do not know which one was modified, validate all.
                  OParams::get_target()->validate();
                  OLSys::get_target()->validate();
                  OMap::get_target()->validate();

                  max_mem_size_ = drawing::memory_size(system_size_);

                  compute_vertices();
              },
              [this]()
              {
                  // Normally, only one was modified (or the user has a
                  // sub-frame auto-clicker), so only one will be reverted.
                  OParams::get_target()->revert();
                  OLSys::get_target()->revert();
                  OMap::get_target()->revert();
              }
            };
        popups_ids_.push_back(procgui::push_popup(size_warning_popup));
    }

    void LSystemView::compute_vertices()
    {
        // Invariant respected: cohesion between the vertices and the bounding
        // boxes.

        const auto& [str, iterations, max_iteration] = OLSys::get_target()->ref_rule_map()->produce(OParams::get_target()->get_n_iter(), system_size_.lsystem_size);
        max_iteration_ = max_iteration;
        turtle_.init_from_parameters(*OParams::get_target());
        turtle_.compute_vertices(str,
                                 iterations,
                                 *OMap::get_target()->get_rule_map(),
                                 system_size_.vertices_size);
        bounding_box_ = geometry::bounding_box(turtle_.vertices_);
        sub_boxes_ = geometry::sub_boxes(turtle_.vertices_, MAX_SUB_BOXES);
        geometry::expand_boxes(sub_boxes_); // Add some margin

        paint_vertices();
    }

    void LSystemView::paint_vertices()
    {
        // un-transformed vertices and bounding box
        OPainter::get_target()->get_target()->paint_vertices(turtle_.vertices_,
                                                             turtle_.iterations_,
                                                             turtle_.transparency_,
                                                             max_iteration_,
                                                             bounding_box_);
        is_modified_ = true;
    }


    void LSystemView::draw(sf::RenderTarget &target)
    {
        // Interact with the models.
        interact_with(*this, name_, is_modified_, &is_selected_);

        sf::FloatRect visible_bounding_box = get_transform().transformRect(bounding_box_);

        // Draw a placeholder if the LSystem does not have enough vertices or
        // does not have any size.
        if (turtle_.vertices_.size() < 2 ||
            (bounding_box_.width < std::numeric_limits<float>::epsilon() &&
             bounding_box_.height < std::numeric_limits<float>::epsilon()))
        {
            draw_missing_placeholder();
            visible_bounding_box = compute_placeholder_box();
        }
        else // Draw the vertices.
        {
            target.draw(turtle_.vertices_.data(),
                        turtle_.vertices_.size(),
                        sf::LineStrip, get_transform());
            OPainter::get_target()->unwrap()->supplementary_drawing(visible_bounding_box);
        }

        if (is_selected_ && bounding_box_is_visible_)
        {
            draw_select_box(target, visible_bounding_box);
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

    void LSystemView::draw_missing_placeholder() const
    {
        auto placeholder_box = compute_placeholder_box();
        const auto& left = placeholder_box.left;
        const auto& top = placeholder_box.top;
        const auto& width = placeholder_box.width;
        const auto& height = placeholder_box.height;

        sf::Color placeholder_color = bw_contrast_color(sfml_window::background_color);
        placeholder_color.a = 150;

        std::vector<sf::Vertex> vertices =
            { {{left, top}, placeholder_color},
              {{left+width, top}, placeholder_color},
              {{left+width, top+height}, placeholder_color},
              {{left, top+height}, placeholder_color},
              {{left, top}, placeholder_color},
              {{left+width, top+height}, placeholder_color}};

        SupplementaryRendering::add_draw_call({vertices, sf::LineStrip});
    }

    sf::FloatRect LSystemView::compute_placeholder_box() const
    {
        sf::FloatRect placeholder_box = get_transform().transformRect(bounding_box_);
        const float ratio = 1/16.f;
        sf::Vector2f window_size = sf::Vector2f(sfml_window::window.getSize()) * controller::WindowController::get_zoom_level();
        float placeholder_size = window_size.x < window_size.y ? window_size.x*ratio : window_size.y*ratio;
        placeholder_box.width = placeholder_size;
        placeholder_box.height = placeholder_size;
        return placeholder_box;
    }

    void LSystemView::draw_select_box(sf::RenderTarget& target, const sf::FloatRect& bounding_box) const
    {
        auto box = bounding_box;
        auto margin = (box.width*.05f > box.height*.05f) ? box.height*.05f : box.width*.05f;
        float zoom = controller::WindowController::get_zoom_level();
        margin = margin > 7.5*zoom ? margin : 7.5*zoom;

        // Draw the global bounding boxes (with a little scaled margin) with
        // the unique color.
        std::array<sf::Vertex, 5> rect =
            {{ {{ box.left-margin, box.top-margin}, color_id_},
               {{ box.left-margin, box.top + box.height + margin}, color_id_},
               {{ box.left + box.width + margin, box.top + box.height + margin}, color_id_},
               {{ box.left + box.width + margin, box.top - margin}, color_id_},
               {{ box.left - margin, box.top - margin}, color_id_}}};
        target.draw(rect.data(), rect.size(), sf::LineStrip);
    }

    bool LSystemView::is_selected() const
    {
        return is_selected_;
    }

    bool LSystemView::is_inside(const sf::Vector2f& click) const
    {
        //  If no placeholder is necessary, checks if 'click' is inside one of
        //  the sub-boxes.
        if (turtle_.vertices_.size() >= 2 &&
            (bounding_box_.width  >= std::numeric_limits<float>::epsilon() ||
             bounding_box_.height >= std::numeric_limits<float>::epsilon()))
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
        else // checks if 'click' is inside the placeholder box.
        {
            return compute_placeholder_box().contains(sf::Vector2f(click));
        }
    }

    void LSystemView::select()
    {
        is_selected_ = true;
    }

    bool LSystemView::box_is_visible() const
    {
        return bounding_box_is_visible_;
    }
    void LSystemView::set_box_visibility(bool is_visible)
    {
        bounding_box_is_visible_ = is_visible;
    }
}
