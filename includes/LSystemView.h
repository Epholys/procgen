#ifndef LSYSTEM_VIEW
#define LSYSTEM_VIEW


#include "cereal/cereal.hpp"
#include "cereal/access.hpp"

#include "geometry.h"
#include "DrawingParameters.h"
#include "LSystemBuffer.h"
#include "InterpretationMapBuffer.h"
#include "UniqueColor.h"
#include "UniqueId.h"
#include "VertexPainterWrapper.h"
#include "VertexPainterSerializer.h"
#include "size_computer.h"

namespace procgui
{

    // This class is the View of the LSystem, InterpretationMap, and
    // DrawingParameters.
    // It manages mainly three things:
    //     - The vertices of the corresponding drawing,
    //     - The interactive GUI of the parameters (with unique identifiers),
    //     - The selection of itself by the user and by extension the bounding
    //     boxes.
    //
    // Invariant:
    //     - The 'turtle_' is in a coherent state with the LSystem,
    //     InterpretationMap, DrawingParameter, and VertexPainter.
    //     - The 'bounding_box_' and 'sub_boxes_' must correspond with the
    //     'vertices_'.
    //     - Each instance as a unique 'id_' and 'color_id_'
    //
    // TODO: simplifies ctor by initializing some attribute here.
    class LSystemView : public Observer<procgui::LSystemBuffer>,
                        public Observer<procgui::InterpretationMapBuffer>,
                        public Observer<drawing::DrawingParameters>,
                        public Observer<colors::VertexPainterWrapper>
    {
    public:
        using OLSys = Observer<procgui::LSystemBuffer>;
        using OMap = Observer<procgui::InterpretationMapBuffer>;
        using OParams = Observer<drawing::DrawingParameters>;
        using OPainter = Observer<colors::VertexPainterWrapper>;

        // Too many moving pieces to serenely have a default constructor.
        // TODO: bite the bullet and do it
        LSystemView() = delete;
        virtual ~LSystemView();
        LSystemView(const std::string& name,
                    std::shared_ptr<LSystem> lsys,
                    std::shared_ptr<drawing::InterpretationMap> map,
                    std::shared_ptr<drawing::DrawingParameters> params,
                    std::shared_ptr<colors::VertexPainterWrapper> painter = std::make_shared<colors::VertexPainterWrapper>());
        // Special-case constructor when creating a default LSystem
        LSystemView(const ext::sf::Vector2d& position, double step);
        // Deep copy;
        //   - All Observers' pointers are cloned or moved
        //   - 'turtle_' is copied
        //   - Id and colors are created or moved
        //   - Selection is reset
        LSystemView(const LSystemView& other);
        LSystemView(LSystemView&& other);
        LSystemView& operator=(const LSystemView& other);
        LSystemView& operator=(LSystemView&& other);

        // -- Reference Getters
        drawing::DrawingParameters& ref_parameters();
        LSystemBuffer& ref_lsystem_buffer();
        InterpretationMapBuffer& ref_interpretation_buffer();
        colors::VertexPainterWrapper& ref_vertex_painter_wrapper();
        // -- Getters:
        // Correctly translated to screen-space bounding_box.
        sf::FloatRect get_bounding_box() const;
        const drawing::DrawingParameters& get_parameters() const;
        const LSystemBuffer& get_lsystem_buffer() const;
        const InterpretationMapBuffer& get_interpretation_buffer() const;
        const colors::VertexPainterWrapper& get_vertex_painter_wrapper() const;
        const drawing::Turtle& get_turtle() const;
        int get_id() const;
        sf::Color get_color() const;

        std::string get_name() const;
        void set_name(const std::string& name);

        void set_headless(bool is_headless);

        bool is_modified() const;

        // Translation transform to correct screen-space position of the
        // LSystem.
        sf::Transform get_transform() const;

        // Getter to is_selected_.
        bool is_selected() const;
        // Select the view.
        void select();

        // Getter and Setter to bounding_box_is_displayed;
        bool box_is_visible() const;
        void set_box_visibility(bool is_visible);

        // Check if 'click' is inside one of the correctly translated
        // 'bounding_box_'.
        bool is_inside(const sf::Vector2f& click) const;

        // Compute the vertices of the turtle interpretation of the LSystem.
        void compute_vertices();
        // Paint the vertices.
        void paint_vertices();

        // Draw the vertices.
        void draw(sf::RenderTarget &target);

        // Should be called after creating a LSystemView:
        //   - Compute vertices
        //   - Call 'center()'
        //   - 'is_modified_' is set to false
        void finish_loading();

    private:
        // Adjust the LSystemView;
        //    - Put its middle in 'starting_position'
        //    - Adjust the scaling so that it takes a certain ratio of the
        //    lowest dimension of the screen size
        void adjust();

        // Update the callbacks of the Observers
        void update_callbacks();

        // Draw a placeholder box if the LSystem does not have enough vertices
        // or does not have any size.
        void draw_missing_placeholder() const;
        // Create the placeholder box.
        sf::FloatRect compute_placeholder_box() const;

        // Draw the box when a LSystemView is selected.
        void draw_select_box(sf::RenderTarget& target, const sf::FloatRect& bounding_box) const;

        // Safeguard the computation of vertices when the size is too big.
        // Flag the opening of the size warning popup if the size is higher than
        void size_safeguard();
        // Display the size warning popup and call 'compute_vertices()' if the
        // user confirms the computation.
        void open_size_warning_popup();

        // The managers of unique identifiers and colors for each instance of
        // LSystemView.
        static UniqueId unique_ids_;
        static colors::UniqueColor unique_colors_;
        // Unique identifier for each instance. Used in procgui.
        int id_;
        // Unique color for each instance. Linked to 'id_'.
        sf::Color color_id_;

        // The window's name.
        std::string name_;

        // true if the LSystem is modified from the last save
        bool is_modified_;

        // The turtle containing the vertices, interations, and max_iteration.
        // Must be coherent with the Observers.
        drawing::Turtle turtle_;

        // The maximum number of iteration of the LSystem for the iteration
        // predecessors.
        u8 max_iteration_;

        // The global bounding box of the drawing. It is a "raw" bounding box:
        // its position is fixed. The rendering at the correct position as well
        // as getters are correctly translated with 'get_transform()'.
        sf::FloatRect bounding_box_;

        // The sub-bounding boxes of the drawing: a more precise way to decide
        // if a mouse click select this View.
        static constexpr int MAX_SUB_BOXES = 8;
        std::vector<sf::FloatRect> sub_boxes_;

        // True if the window is selected.
        bool is_selected_;
        // True if the bounding box must be visible
        bool bounding_box_is_visible_;

        // RAM size of the data the user want to compute
        drawing::system_size system_size_ = {0, 0};
        drawing::Matrix::number max_mem_size_ {0};

        // Ids list of all created popups, existing or deleted.
        std::vector<int> popups_ids_;

        // Flag to call 'center()'
        bool to_adjust_ {false};

        bool headless {false};

        // Serialization
        friend class cereal::access;

        template<class Archive>
        void save (Archive& ar, const u32) const
            {
                ar(cereal::make_nvp("name", name_),
                   cereal::make_nvp("LSystem", *OLSys::get_target()->get_rule_map()),
                   cereal::make_nvp("DrawingParameters", *OParams::get_target()),
                   cereal::make_nvp("Interpretation Map", *OMap::get_target()->get_rule_map()));

                auto painter_serializer  = colors::VertexPainterSerializer(OPainter::get_target()->unwrap());
                ar(cereal::make_nvp("VertexPainter", painter_serializer));
            }

        template<class Archive>
        void load (Archive& ar, const u32)
            {
                std::string name;
                LSystem lsys;
                drawing::DrawingParameters params;
                drawing::InterpretationMap map;
                colors::VertexPainterSerializer painter_serializer;

                ar(name,
                   cereal::make_nvp("LSystem", lsys),
                   cereal::make_nvp("DrawingParameters", params),
                   cereal::make_nvp("Interpretation Map", map),
                   cereal::make_nvp("VertexPainter", painter_serializer));


                *this = LSystemView(name,
                                    std::make_shared<LSystem>(lsys),
                                    std::make_shared<drawing::InterpretationMap>(map),
                                    std::make_shared<drawing::DrawingParameters>(params),
                                    std::make_shared<colors::VertexPainterWrapper>(painter_serializer.get_serialized()));

            }
    };
}

#endif
