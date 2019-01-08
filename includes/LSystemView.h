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
#include "VertexPainterBuffer.h"

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
    //     - The 'vertices_' are at any time painted with VertexPainter
    //     - The 'bounding_box_' and 'sub_boxes_' must correspond with the
    //     'vertices_'.
    //
    // Note:
    //    - LSystemView contain a shared ownership of the LSystem and the
    //    InterpretationMap via the corresponding Observer. As a consequence, a
    //    copy of LSystemView will share the same LSystem and Map.
    class LSystemView : public Observer<LSystem>,
                        public Observer<drawing::InterpretationMap>,
                        public Observer<drawing::DrawingParameters>,
                        public Observer<colors::VertexPainterBuffer>
    {
    public:
        using OLSys = Observer<LSystem>;
        using OMap = Observer<drawing::InterpretationMap>;
        using OParams = Observer<drawing::DrawingParameters>;
        using OPainter = Observer<colors::VertexPainterBuffer>;

        LSystemView(const std::string& name,
                    std::shared_ptr<LSystem> lsys,
                    std::shared_ptr<drawing::InterpretationMap> map,
                    std::shared_ptr<drawing::DrawingParameters> params,
                    std::shared_ptr<colors::VertexPainterBuffer> painter = std::make_shared<colors::VertexPainterBuffer>());
        explicit LSystemView(const ext::sf::Vector2d& position);
        // Shallow copy: LSystem, DrawingParameters and VertexPainterBuffer
        // are shared from 'other'. Use 'clone()' for a deep copy.
        LSystemView(const LSystemView& other);
        LSystemView(LSystemView&& other);
        LSystemView& operator=(const LSystemView& other);
        LSystemView& operator=(LSystemView&& other);
        ~LSystemView();
        
        // Clone the LSystemView into an independant other view: deep copy.
        LSystemView clone() const;

        // Shallow-copy 'this'.
        LSystemView duplicate() const;
        
        // Reference Getters
        drawing::DrawingParameters& ref_parameters();
        LSystemBuffer& ref_lsystem_buffer();
        InterpretationMapBuffer& ref_interpretation_buffer();
        colors::VertexPainterBuffer& ref_vertex_painter_buffer();
        // Getters
        // Correctly translated to screen-space bounding_box.
        sf::FloatRect get_bounding_box() const;
        const drawing::DrawingParameters& get_parameters() const;
        const LSystemBuffer& get_lsystem_buffer() const;
        const InterpretationMapBuffer& get_interpretation_buffer() const;
        const colors::VertexPainterBuffer& get_vertex_painter_buffer() const;
        int get_id() const;
        sf::Color get_color() const;
        // Translation transform to correct screen-space position of the
        // LSystem. 
        sf::Transform get_transform() const;

        // Compute the vertices of the turtle interpretation of the LSystem.
        void compute_vertices();
        void paint_vertices();

        // Draw the vertices.
        void draw(sf::RenderTarget &target);

        // Getter to is_selected_.
        bool is_selected() const;

        // Check if 'click' is inside one of the correctly translated
        // 'bounding_box_'. 
        bool is_inside(const sf::Vector2f& click) const;

        // Select the view.
        void select();

                
    private:
        void update_callbacks();
        
        // // // Unique identifier for each instance (with a growing id_count_).
        // // // Used in the GUI.
        // // static int id_count_;
        // int id_;
        // // The unique color generator. Each id is associated with a unique
        // // color. Static as shared between every objects.
        // static colors::UniqueColor color_gen_;
        // sf::Color color_id_; // Color associated to the id.
        static UniqueId unique_ids_;
        static colors::UniqueColor unique_colors_;
        int id_;
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

        // Serialization
        friend class cereal::access;

        template<class Archive>
        void save (Archive& ar, const std::uint32_t) const
            {
                ar(cereal::make_nvp("name", name_),
                   cereal::make_nvp("LSystem", *Observer<LSystem>::get_target()),
                   cereal::make_nvp("DrawingParameters", *Observer<drawing::DrawingParameters>::get_target()),
                   cereal::make_nvp("Interpretation Map", *Observer<drawing::InterpretationMap>::get_target()));
            }

        template<class Archive>
        void load (Archive& ar, const std::uint32_t)
            {
                std::string name;
                LSystem lsys;
                drawing::DrawingParameters params;
                drawing::InterpretationMap map;

                ar(name,
                   cereal::make_nvp("LSystem", lsys),
                   cereal::make_nvp("DrawingParameters", params),
                   cereal::make_nvp("Interpretation Map", map));

                *this = LSystemView(name,
                                    std::make_shared<LSystem>(lsys),
                                    std::make_shared<drawing::InterpretationMap>(map),
                                    std::make_shared<drawing::DrawingParameters>(params));

            }
    };
}

#endif
