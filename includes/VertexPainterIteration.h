#ifndef VERTEX_PAINTER_ITERATION_H
#define VERTEX_PAINTER_ITERATION_H


#include "VertexPainter.h"

namespace colors
{
    class VertexPainterIteration : public VertexPainter
    {
    public:
        VertexPainterIteration(); // Create a default generator
        explicit VertexPainterIteration(const std::shared_ptr<ColorGenerator> gen);
        // Shallow rule-of-five constructors.
        VertexPainterIteration(const VertexPainterIteration& other);
        VertexPainterIteration(VertexPainterIteration&& other);
        VertexPainterIteration& operator=(const VertexPainterIteration& other);
        VertexPainterIteration& operator=(VertexPainterIteration&& other);
        
        // Paint 'vertices' according to its iteration value: simply divide the
        // current iteration by the max iteration.
        // 'bounding_box' is not used.
        virtual void paint_vertices(std::vector<sf::Vertex>& vertices,
                                    const std::vector<int>& vertices_iteration,
                                    int max_iteration,
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

CEREAL_REGISTER_TYPE_WITH_NAME(colors::VertexPainterIteration, "VertexPainterIteration");
CEREAL_REGISTER_POLYMORPHIC_RELATION(colors::VertexPainter, colors::VertexPainterIteration)


#endif // VERTEX_PAINTER_ITERATION_H
