#ifndef VERTEX_PAINTER_CONSTANT_H
#define VERTEX_PAINTER_CONSTANT_H


#include "VertexPainter.h"

namespace colors
{
    class VertexPainterConstant : public VertexPainter
    {
    public:
        VertexPainterConstant(); // Create a default generator
        explicit VertexPainterConstant(const std::shared_ptr<ColorGeneratorWrapper> wrapper);
        // TODO doc "slicing"? Shallow rule-of-five constructors.
        VertexPainterConstant(const VertexPainterConstant& other) = delete;
        VertexPainterConstant(VertexPainterConstant&& other) = delete;
        VertexPainterConstant& operator=(const VertexPainterConstant& other) = delete;
        VertexPainterConstant& operator=(VertexPainterConstant&& other) = delete;
        
        // Paint 'vertices' according to a constant real number.
        // 'bounding_box', 'iteration_of_vertices' and 'max_recursion' are not used.
        virtual void paint_vertices(std::vector<sf::Vertex>& vertices,
                                    const std::vector<int>& iteration_of_vertices,
                                    int max_recursion,
                                    sf::FloatRect bounding_box) override;

    private:
        // Implements the deep-copy cloning.
        virtual std::shared_ptr<VertexPainter> clone_impl() const override;

        friend class cereal::access;
        template<class Archive>
        void save(Archive& ar, const std::uint32_t) const
            {
                ar(cereal::make_nvp("ColorGenerator", get_generator_wrapper()->unwrap()));
            }
        template<class Archive>
        void load(Archive& ar, const std::uint32_t)
            {
                std::shared_ptr<ColorGenerator> generator;
                ar(cereal::make_nvp("ColorGenerator", generator));
                set_generator_wrapper(std::make_shared<ColorGeneratorWrapper>(generator));
            }
    };
}

CEREAL_REGISTER_TYPE_WITH_NAME(colors::VertexPainterConstant, "VertexPainterConstant");
CEREAL_REGISTER_POLYMORPHIC_RELATION(colors::VertexPainter, colors::VertexPainterConstant)

#endif // VERTEX_PAINTER_CONSTANT_H
