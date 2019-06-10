#ifndef VERTEX_PAINTER_LINEAR_H
#define VERTEX_PAINTER_LINEAR_H

#include "VertexPainter.h"
#include "helper_cereal.hpp"
#include "ColorsGeneratorSerializer.h"

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
        sf::Vector2f get_center() const;
        
        // Setter
        void set_angle(float angle);
        void set_center(sf::Vector2f center);
        
        // Paint 'vertices' following a line passing through the center at a
        // certain 'angle_' according to the informations of 'bounding_box'
        // according to the rule with the colors from the ColorGenerator.
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

        float angle_ {0};
        sf::Vector2f center_ {0.5,0.5};
        
        friend class cereal::access;
        template<class Archive>
        void save(Archive& ar, const std::uint32_t) const
            {
                ar(cereal::make_nvp("angle", angle_),
                   cereal::make_nvp("center", center_));
                
                auto color_generator = get_generator_wrapper()->unwrap();
                auto serializer = ColorGeneratorSerializer(color_generator);
                ar(cereal::make_nvp("ColorGenerator", serializer));
            }
        template<class Archive>
        void load(Archive& ar, const std::uint32_t)
            {
                ar(cereal::make_nvp("angle", angle_));
                ar(cereal::make_nvp("center", center_));

                ColorGeneratorSerializer serializer;
                ar(cereal::make_nvp("ColorGenerator", serializer));
                set_generator_wrapper(std::make_shared<ColorGeneratorWrapper>(serializer.get_serialized()));
            }
    };
}


#endif // VERTEX_PAINTER_LINEAR_H
