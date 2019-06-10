#ifndef VERTEX_PAINTER_SEQUENTIAL_H
#define VERTEX_PAINTER_SEQUENTIAL_H


#include "VertexPainter.h"
#include "ColorsGeneratorSerializer.h"

namespace colors
{
    class VertexPainterSequential : public VertexPainter
    {
    public:
        VertexPainterSequential(); // Create a default generator
        explicit VertexPainterSequential(const std::shared_ptr<ColorGeneratorWrapper> wrapper);
        virtual ~VertexPainterSequential() {}
        // This class is mainly used polymorphic-ally, so deleting these
        // constructors saved some LoC so potential bugs.
        VertexPainterSequential(const VertexPainterSequential& other) = delete;
        VertexPainterSequential(VertexPainterSequential&& other) = delete;
        VertexPainterSequential& operator=(const VertexPainterSequential& other) = delete;
        VertexPainterSequential& operator=(VertexPainterSequential&& other) = delete;
        
        // Getters
        float get_factor() const;

        // Setter
        void set_factor(float factor);
        
        // Paint 'vertices' according to the order of the vertices in the
        // 'vertices' vector.
        // 'bounding_box', 'iteration_of_vertices' and 'max_recursion' are not used.
        virtual void paint_vertices(std::vector<sf::Vertex>& vertices,
                                    const std::vector<int>& iteration_of_vertices,
                                    int max_recursion,
                                    sf::FloatRect bounding_box) override;

        // Implements the deep-copy cloning.
        virtual std::shared_ptr<VertexPainter> clone() const override;

        friend class VertexPainterSerializer;
        virtual std::string type_name() const override;        

    private:
        friend class cereal::access;
        template<class Archive>
        void save(Archive& ar, const std::uint32_t) const
            {
                ar(cereal::make_nvp("repetition_factor", factor_));

                auto color_generator = get_generator_wrapper()->unwrap();
                auto serializer = ColorGeneratorSerializer(color_generator);
                ar(cereal::make_nvp("ColorGenerator", serializer));

            }
        template<class Archive>
        void load(Archive& ar, const std::uint32_t)
            {
                ar(cereal::make_nvp("repetition_factor", factor_));

                ColorGeneratorSerializer serializer;
                ar(cereal::make_nvp("ColorGenerator", serializer));
                set_generator_wrapper(std::make_shared<ColorGeneratorWrapper>(serializer.get_serialized()));
            }

        float factor_ {0};
    };
}


#endif // VERTEX_PAINTER_SEQUENTIAL_H
