#ifndef VERTEX_PAINTER_RADIAL_H
#define VERTEX_PAINTER_RADIAL_H


#include "VertexPainter.h"
#include "helper_cereal.hpp"
#include "ColorsGeneratorSerializer.h"
#include "WindowController.h"

namespace colors
{
    class VertexPainterRadial : public VertexPainter
    {
    public:
        VertexPainterRadial(); // Create a default generator
        explicit VertexPainterRadial(const std::shared_ptr<ColorGeneratorWrapper> wrapper);
        virtual ~VertexPainterRadial() {}
        // This class is mainly used polymorphic-ally, so deleting these
        // constructors saved some LoC so potential bugs.
        VertexPainterRadial(const VertexPainterRadial& other) = delete;
        VertexPainterRadial(VertexPainterRadial&& other) = delete;
        VertexPainterRadial& operator=(const VertexPainterRadial& other) = delete;
        VertexPainterRadial& operator=(VertexPainterRadial&& other) = delete;
        
        // Getters
        sf::Vector2f get_center() const;
        bool get_display_flag() const;

        // Setter
        void set_center(sf::Vector2f center);
        void set_display_flag(bool flag);

        // Display a circle representing the position of 'circle_'
        virtual void supplementary_drawing(sf::FloatRect bounding_box) const override;
        
        // Paint 'vertices' in a 'center_' centered distance-bases radial
        // fashion with the informations of 'bounding_box' according to the rule
        // with the colors from the ColorGenerator.
        // 'iteration_of_vertices' and 'max_recursion' are not used.
        virtual void paint_vertices(std::vector<sf::Vertex>& vertices,
                                    const std::vector<int>& iteration_of_vertices,
                                    int max_recursion,
                                    sf::FloatRect bounding_box) override;

        // Implements the deep-copy cloning.
        virtual std::shared_ptr<VertexPainter> clone() const override;

        friend class VertexPainterSerializer;
        virtual std::string type_name() const override;        
        
    private:
        // [0, 1] center: 0 is the left/up of the bounding_box and 1 the right/down.
        sf::Vector2f center_ {0, 0};
        bool display_helper_ {true};
        
        friend class cereal::access;
        template<class Archive>
        void save(Archive& ar, const std::uint32_t) const
            {
                // ar(cereal::make_nvp("center", center_),
                //    cereal::make_nvp("ColorGenerator", get_generator_wrapper()->unwrap()));
                ar(cereal::make_nvp("center", center_));
                auto color_generator = get_generator_wrapper()->unwrap();
                auto serializer = ColorGeneratorSerializer(color_generator);
                ar(cereal::make_nvp("ColorGenerator", serializer));
            }
        template<class Archive>
        void load(Archive& ar, const std::uint32_t)
            {
                ar(cereal::make_nvp("center", center_));             

                if (center_.x < 0. || center_.x > 1. ||
                    center_.y < 0. || center_.y > 1.)
                {
                    center_.x = std::clamp(center_.x, 0.f, 1.f);
                    center_.y = std::clamp(center_.y, 0.f, 1.f);
                    controller::WindowController::add_loading_error_message("VertexPainterRadial's center's coordinates weren't in the [0,1] range, so they are clamped.");
                }

                ColorGeneratorSerializer serializer;
                ar(cereal::make_nvp("ColorGenerator", serializer));
                set_generator_wrapper(std::make_shared<ColorGeneratorWrapper>(serializer.get_serialized()));
            }
    };
}


#endif // VERTEX_PAINTER_RADIAL_H
