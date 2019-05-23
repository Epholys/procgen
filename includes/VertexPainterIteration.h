#ifndef VERTEX_PAINTER_ITERATION_H
#define VERTEX_PAINTER_ITERATION_H


#include "VertexPainter.h"

namespace colors
{
    class VertexPainterIteration : public VertexPainter
    {
    public:
        VertexPainterIteration(); // Create a default generator
        explicit VertexPainterIteration(const std::shared_ptr<ColorGeneratorWrapper> wrapper);
        // This class is mainly used polymorphic-ally, so deleting these
        // constructors saved some LoC so potential bugs.
        VertexPainterIteration(const VertexPainterIteration& other) = delete;
        VertexPainterIteration(VertexPainterIteration&& other) = delete;
        VertexPainterIteration& operator=(const VertexPainterIteration& other) = delete;
        VertexPainterIteration& operator=(VertexPainterIteration&& other) = delete;
        
        // Paint 'vertices' according to its iteration value: simply divide the
        // current iteration by the max iteration.
        // 'bounding_box' is not used.
        virtual void paint_vertices(std::vector<sf::Vertex>& vertices,
                                    const std::vector<int>& vertices_iteration,
                                    int max_iteration,
                                    sf::FloatRect bounding_box) override;

        // Implements the deep-copy cloning.
        virtual std::shared_ptr<VertexPainter> clone() const override;

    private:
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

#endif // VERTEX_PAINTER_ITERATION_H
