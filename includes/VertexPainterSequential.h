#ifndef VERTEX_PAINTER_SEQUENTIAL_H
#define VERTEX_PAINTER_SEQUENTIAL_H


#include "VertexPainter.h"

namespace colors
{
    class VertexPainterSequential : public VertexPainter
    {
    public:
        VertexPainterSequential(); // Create a default generator
        explicit VertexPainterSequential(const std::shared_ptr<ColorGenerator> gen);
        // Shallow rule-of-five constructors.
        VertexPainterSequential(const VertexPainterSequential& other);
        VertexPainterSequential(VertexPainterSequential&& other);
        VertexPainterSequential& operator=(const VertexPainterSequential& other);
        VertexPainterSequential& operator=(VertexPainterSequential&& other);
        
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


    private:
        // Implements the deep-copy cloning.
        virtual std::shared_ptr<VertexPainter> clone_impl() const override;

        friend class cereal::access;
        template<class Archive>
        void save(Archive& ar) const
            {
                ar(cereal::make_nvp("repetition_factor", factor_),
                   cereal::make_nvp("ColorGenerator", get_generator_wrapper()->unwrap()));
            }
        template<class Archive>
        void load(Archive& ar)
            {
                std::shared_ptr<ColorGenerator> generator;
                ar(cereal::make_nvp("repetition_factor", factor_));
                ar(cereal::make_nvp("ColorGenerator", generator));
                set_generator_wrapper(std::make_shared<ColorGeneratorWrapper>(generator));
            }

        float factor_ {0};
    };
}

CEREAL_REGISTER_TYPE_WITH_NAME(colors::VertexPainterSequential, "VertexPainterSequential");
CEREAL_REGISTER_POLYMORPHIC_RELATION(colors::VertexPainter, colors::VertexPainterSequential)


#endif // VERTEX_PAINTER_SEQUENTIAL_H
