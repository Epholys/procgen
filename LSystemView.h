#ifndef LSYSTEM_VIEW
#define LSYSTEM_VIEW


#include "geometry.h"
#include "DrawingParameters.h"
#include "LSystemBuffer.h"
#include "InterpretationMapBuffer.h"

namespace procgui
{
    class LSystemView : public Observer<LSystem>,
                        public Observer<drawing::InterpretationMap>
    {
    public:
        LSystemView(std::shared_ptr<LSystem> lsys,
                    std::shared_ptr<drawing::InterpretationMap> map,
                    drawing::DrawingParameters param);

        //Getters
        drawing::DrawingParameters& get_parameters();
        LSystemBuffer& get_lsystem_buffer();
        InterpretationMapBuffer& get_interpretation_buffer();

        
        // Compute the vertices of the turtle interpretation of the LSystem.
        void compute_vertices();

        // Draw the vertices.
        void draw (sf::RenderTarget &target);
        
    private:        
        LSystemBuffer lsys_buff_;
        InterpretationMapBuffer interpretation_buff_;
        drawing::DrawingParameters params_;

        std::vector<sf::Vertex> vertices_;
        sf::FloatRect bounding_box_;
        static constexpr int MAX_SUB_BOXES = 8;
        std::vector<sf::FloatRect> sub_boxes_;
    };
}

#endif
