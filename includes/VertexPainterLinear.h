#ifndef VERTEX_PAINTER_LINEAR_H
#define VERTEX_PAINTER_LINEAR_H

#include "VertexPainter.h"
#include "helper_cereal.hpp"
#include "ColorsGeneratorSerializer.h"
#include "helper_math.h"
#include "LoadMenu.h"

namespace colors
{
    // Paint the vertices with a ColorGenerator following a line passing through
    // the center of the bounding box at a certain 'angle_'.
    class VertexPainterLinear : public VertexPainter
    {
    public:
        VertexPainterLinear(); // Create a default generator
        explicit VertexPainterLinear(const std::shared_ptr<ColorGeneratorWrapper> wrapper);
        virtual ~VertexPainterLinear() {}
        // This class is mainly used polymorphic-ally, so deleting these
        // constructors saved some LoC so potential bugs.
        VertexPainterLinear(const VertexPainterLinear& other) = delete;
        VertexPainterLinear(VertexPainterLinear&& other) = delete;
        VertexPainterLinear& operator=(const VertexPainterLinear& other) = delete;
        VertexPainterLinear& operator=(VertexPainterLinear&& other) = delete;

        // Getters
        float get_angle() const;
        bool get_display_flag() const;

        // Setter
        void set_angle(float angle);
        void set_display_flag(bool flag);

        // Paint 'vertices' following a line passing through the center at a
        // certain 'angle_' according to the informations of 'bounding_box'
        // according to the rule with the colors from the ColorGenerator.
        // 'iteration_of_vertices' and 'max_recursion' are not used.
        virtual void paint_vertices(std::vector<sf::Vertex>& vertices,
                                    const std::vector<u8>& iteration_of_vertices,
                                    const std::vector<bool>& transparent,
                                    int max_recursion,
                                    sf::FloatRect bounding_box) override;

        // Implements the deep-copy cloning.
        virtual std::shared_ptr<VertexPainter> clone() const override;

        friend class VertexPainterSerializer;
        virtual std::string type_name() const override;

        // Display a line representing 'angle_'
        virtual void supplementary_drawing(sf::FloatRect bounding_box) const override;

    private:

        float angle_ {0};
        sf::Vector2f center_ {0.5,0.5};
        bool display_helper_ {true};

        friend class cereal::access;
        template<class Archive>
        void save(Archive& ar, const u32) const
            {
                ar(cereal::make_nvp("angle", angle_));

                auto color_generator = get_generator_wrapper()->unwrap();
                auto serializer = ColorGeneratorSerializer(color_generator);
                ar(cereal::make_nvp("ColorGenerator", serializer));
            }
        template<class Archive>
        void load(Archive& ar, const u32)
            {
                ar(cereal::make_nvp("angle", angle_));

                if (angle_ < 0. || angle_ > 360)
                {
                    angle_ = math::clamp_angle(angle_);
                    controller::LoadMenu::add_loading_error_message("VertexPainterLinear's angle wasn't in the [0,360] range, so it is clamped.");
                }

                ColorGeneratorSerializer serializer;
                ar(cereal::make_nvp("ColorGenerator", serializer));
                set_generator_wrapper(std::make_shared<ColorGeneratorWrapper>(serializer.get_serialized()));
            }
    };
}


#endif // VERTEX_PAINTER_LINEAR_H
