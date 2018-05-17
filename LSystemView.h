#ifndef LSYSTEM_VIEW
#define LSYSTEM_VIEW


#include "cereal/cereal.hpp"
#include "cereal/access.hpp"

#include "geometry.h"
#include "DrawingParameters.h"
#include "LSystemBuffer.h"
#include "InterpretationMapBuffer.h"

namespace procgui
{

    // This class is the View of the LSystem, InterpretationMap, and
    // DrawingParameters.
    // It manages mainly three things:
    //     - The vertices of the corresponding drawing,
    //     - The interactive GUI of the parameters,
    //     - The selection of itself by the user and by extension the bounding
    //     boxes.
    //
    // Invariant:
    //     - The 'vertices_' must correspond to the 'lsys_buff_',
    //     'interpretation_buff_', and 'params_'.
    //     - The 'bounding_box_' and 'sub_boxes_' must correspond with the
    //     'vertices_'.
    // 
    // Note:
    //    - LSystemView contain a shared ownership of the LSystem and the
    //    InterpretationMap via the corresponding Observer. As a consequence, a
    //    copy of LSystemView will share the same LSystem and Map.
    class LSystemView : public Observer<LSystem>,
                        public Observer<drawing::InterpretationMap>
    {
    public:
        LSystemView(const std::string& name,
                    std::shared_ptr<LSystem> lsys,
                    std::shared_ptr<drawing::InterpretationMap> map,
                    drawing::DrawingParameters param);
        // Create a default LSystemView at 'position'.
        LSystemView(const sf::Vector2f& position);
        // The rule-of-five is necessary with the 'Observer<>' callbacks
        // behaviour. Deep-copy.
        LSystemView(const LSystemView& other);
        LSystemView& operator=(LSystemView other);
        LSystemView(LSystemView&& other);

        // Clone the LSystemView into an independant other view.
        LSystemView clone();

        LSystemView duplicate();
        
        // Reference Getters
        drawing::DrawingParameters& ref_parameters();
        LSystemBuffer& ref_lsystem_buffer();
        InterpretationMapBuffer& ref_interpretation_buffer();
        // Getters
        sf::FloatRect get_bounding_box() const;
        const drawing::DrawingParameters& get_parameters() const;
        const LSystemBuffer& get_lsystem_buffer() const;
        const InterpretationMapBuffer& get_interpretation_buffer() const;
        int get_id() const;
        
        // Compute the vertices of the turtle interpretation of the LSystem.
        void compute_vertices();

        // Draw the vertices.
        void draw(sf::RenderTarget &target);

        // Getter to is_selected_.
        bool is_selected() const;

        // Check if 'click' is inside one of the 'bounding_box_'
        bool is_inside(const sf::Vector2f& click) const;

        // Select the view.
        void select();

                
    private:
        // Used in assignment and move operator.
        void swap(LSystemView& other);

        // Unique identifier for each instance (with a growing id_count_).
        // Used in the GUI
        static int id_count_;
        int id_;

        // The window's name.
        std::string name_;

        // The LSystem's buffer and by extension the LSystem (with shared
        // ownership). 
        LSystemBuffer lsys_buff_;

        // The InterpretationMap's buffer and by extension the
        // InterpretationMap (with shared ownership).
        InterpretationMapBuffer interpretation_buff_;

        // The DrawingParameters (single Ownership)
        drawing::DrawingParameters params_;

        // The vertices of the View. Computer at each modification.
        std::vector<sf::Vertex> vertices_;

        // The global bounding box of the drawing.
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
                ar(cereal::make_nvp("LSystem", *Observer<LSystem>::get_target()),
                   cereal::make_nvp("DrawingParameters", params_),
                   cereal::make_nvp("Interpretation Map", *Observer<drawing::InterpretationMap>::get_target()));
            }

        template<class Archive>
        void load (Archive& ar, const std::uint32_t)
            {
                LSystem lsys;
                drawing::DrawingParameters params;
                drawing::InterpretationMap map;
                ar(cereal::make_nvp("LSystem", lsys),
                   cereal::make_nvp("DrawingParameters", params),
                   cereal::make_nvp("Interpretation Map", map));
                *this = LSystemView("",
                          std::make_shared<LSystem>(lsys),
                          std::make_shared<drawing::InterpretationMap>(map),
                          params);
            }
    };
}

#endif
