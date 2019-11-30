#ifndef VERTEX_PAINTER_CONSTANT_H
#define VERTEX_PAINTER_CONSTANT_H


#include "VertexPainter.h"
#include "ColorsGeneratorSerializer.h"

namespace colors
{
    class VertexPainterConstant : public VertexPainter
    {
    public:
        VertexPainterConstant(); // Create a default generator
        explicit VertexPainterConstant(const std::shared_ptr<ColorGeneratorWrapper> wrapper);
        virtual ~VertexPainterConstant() {}
        // This class is mainly used polymorphic-ally, so deleting these
        // constructors saved some LoC so potential bugs.
        VertexPainterConstant(const VertexPainterConstant& other) = delete;
        VertexPainterConstant(VertexPainterConstant&& other) = delete;
        VertexPainterConstant& operator=(const VertexPainterConstant& other) = delete;
        VertexPainterConstant& operator=(VertexPainterConstant&& other) = delete;

        // Paint 'vertices' according to a constant real number.
        // 'bounding_box', 'iteration_of_vertices' and 'max_recursion' are not used.
        virtual void paint_vertices(std::vector<sf::Vertex>& vertices,
                                    const std::vector<u8>& iteration_of_vertices,
                                    const std::vector<bool>& transparent,
                                    int max_recursion,
                                    sf::FloatRect bounding_box) override;
        // Implements the deep-copy cloning.
        virtual std::shared_ptr<VertexPainter> clone() const override;

        friend class VertexPainterSerializer;
        virtual std::string type_name() const override;

    private:
        friend class cereal::access;
        template<class Archive>
        void save(Archive& ar, const u32) const
            {
                auto color_generator = get_generator_wrapper()->unwrap();
                auto serializer = ColorGeneratorSerializer(color_generator);
                ar(cereal::make_nvp("ColorGenerator", serializer));
            }
        template<class Archive>
        void load(Archive& ar, const u32)
            {
                ColorGeneratorSerializer serializer;
                ar(cereal::make_nvp("ColorGenerator", serializer));
                set_generator_wrapper(std::make_shared<ColorGeneratorWrapper>(serializer.get_serialized()));
            }
    };
}

#endif // VERTEX_PAINTER_CONSTANT_H
