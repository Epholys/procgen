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
    //     - The 'vertices_' and 'iteration_of_vertices_' must correspond to the
    //     LSystem, InterpretationMap, and DrawingParameters.
    //     - The 'vertices_' are at any time painted with VertexPainter.
    //     - The 'bounding_box_' and 'sub_boxes_' must correspond with the
    //     'vertices_'.
    //     - Each instance as a unique 'id_' and 'color_id_'
    //
    // Note:
    //    - LSystemView contain a shared ownership of the LSystem and the
    //    InterpretationMap via the corresponding Observer. As a consequence, a
    //    copy of LSystemView will share the same LSystem and Map.
    class LSystemView : public Observer<LSystem>,
                        public Observer<drawing::InterpretationMap>,
                        public Observer<drawing::DrawingParameters>,
                        public Observer<colors::VertexPainterWrapper>
    {
    public:
        using OLSys = Observer<LSystem>;
        using OMap = Observer<drawing::InterpretationMap>;
        using OParams = Observer<drawing::DrawingParameters>;
        using OPainter = Observer<colors::VertexPainterWrapper>;

        // Too many moving pieces to serenely have a default constructor.
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
        //   - These pointers are set to the RuleMapBuffers (LSys and Map)
        //   - Id and colors are created or moved
        //   - Selection is reset
        LSystemView(const LSystemView& other);
        LSystemView(LSystemView&& other);
        LSystemView& operator=(const LSystemView& other);
        LSystemView& operator=(LSystemView&& other);
        
        // Reference Getters
        drawing::DrawingParameters& ref_parameters();
        LSystemBuffer& ref_lsystem_buffer();
        InterpretationMapBuffer& ref_interpretation_buffer();
        colors::VertexPainterWrapper& ref_vertex_painter_wrapper();
        // Getters
        // Correctly translated to screen-space bounding_box.
        sf::FloatRect get_bounding_box() const;
        const drawing::DrawingParameters& get_parameters() const;
        const LSystemBuffer& get_lsystem_buffer() const;
        const InterpretationMapBuffer& get_interpretation_buffer() const;
        const colors::VertexPainterWrapper& get_vertex_painter_wrapper() const;
        int get_id() const;
        sf::Color get_color() const;

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
        void paint_vertices();

        // Draw the vertices.
        void draw(sf::RenderTarget &target);

                
    private:
        void update_callbacks();

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

        // The LSystem's buffer. It has shared ownership of a
        // shared_ptr<LSystem> with the associated Observable.
        LSystemBuffer lsys_buff_;

        // The InterpretationMap's buffer. It has shared ownership of a
        // shared_ptr<InterpretationMap> with the associated Observable.
        InterpretationMapBuffer interpretation_buff_;

        // The vertices of the View and their iteration count. Computed at each
        // modification.
        std::vector<sf::Vertex> vertices_;
        std::vector<int> iteration_of_vertices_;
        int max_iteration_;
        
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

        // Serialization
        friend class cereal::access;

        template<class Archive>
        void save (Archive& ar, const std::uint32_t) const
            {
                ar(cereal::make_nvp("name", name_),
                   cereal::make_nvp("LSystem", *OLSys::get_target()),
                   cereal::make_nvp("DrawingParameters", *OParams::get_target()),
                   cereal::make_nvp("Interpretation Map", *OMap::get_target()),
                   cereal::make_nvp("VertexPainter", OPainter::get_target()->unwrap()));
            }

        template<class Archive>
        void load (Archive& ar, const std::uint32_t)
            {
                std::string name;
                LSystem lsys;
                drawing::DrawingParameters params;
                drawing::InterpretationMap map;
                std::shared_ptr<colors::VertexPainter> painter;                
                
                ar(name,
                   cereal::make_nvp("LSystem", lsys),
                   cereal::make_nvp("DrawingParameters", params),
                   cereal::make_nvp("Interpretation Map", map),
                   cereal::make_nvp("VertexPainter", painter));

                
                *this = LSystemView(name,
                                    std::make_shared<LSystem>(lsys),
                                    std::make_shared<drawing::InterpretationMap>(map),
                                    std::make_shared<drawing::DrawingParameters>(params),
                                    std::make_shared<colors::VertexPainterWrapper>(painter));

            }
    };
}

#endif
