#ifndef LSYSTEM_VIEW
#define LSYSTEM_VIEW


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
    //     - The 'bounding_box_' and 'sub_boxes_' myst correspond with teh
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
        LSystemView(std::shared_ptr<LSystem> lsys,
                    std::shared_ptr<drawing::InterpretationMap> map,
                    drawing::DrawingParameters param);
        LSystemView(const LSystemView& other);
        LSystemView& operator=(const LSystemView& other);

        // Reference Getters
        drawing::DrawingParameters& get_parameters();
        LSystemBuffer& get_lsystem_buffer();
        InterpretationMapBuffer& get_interpretation_buffer();

        
        // Compute the vertices of the turtle interpretation of the LSystem.
        void compute_vertices();

        // Draw the vertices.
        void draw(sf::RenderTarget &target);

        bool select(const sf::Vector2i& click);
        
    private:        
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

        bool is_selected_;
    };
}

#endif
