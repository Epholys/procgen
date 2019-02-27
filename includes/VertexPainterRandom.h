#ifndef VERTEX_PAINTER_RANDOM_H
#define VERTEX_PAINTER_RANDOM_H


#include "VertexPainter.h"
#include "helper_math.h"

namespace colors
{
    class VertexPainterRandom : public VertexPainter
    {
    public:
        VertexPainterRandom(); // Create a default generator
        explicit VertexPainterRandom(const std::shared_ptr<ColorGenerator> gen);
        // Shallow rule-of-five constructors.
        VertexPainterRandom(const VertexPainterRandom& other);
        VertexPainterRandom(VertexPainterRandom&& other);
        VertexPainterRandom& operator=(const VertexPainterRandom& other);
        VertexPainterRandom& operator=(VertexPainterRandom&& other);
        
        // Generate a new seed.
        void randomize();

        // Getter/Setter
        int get_block_size() const;
        void set_block_size(int block_size);

        // Paint 'vertices' according to a random real number.
        // 'bounding_box', 'iteration_of_vertices' and 'max_recursion' are not used.
        virtual void paint_vertices(std::vector<sf::Vertex>& vertices,
                                    const std::vector<int>& iteration_of_vertices,
                                    int max_recursion,
                                    sf::FloatRect bounding_box) override;

    private:
        // Implements the deep-copy cloning.
        virtual std::shared_ptr<VertexPainter> clone_impl() const override;

        // The number of consecutive vertices to paint the same color.
        // Invariant: must be strictly positive
        int block_size_;
        // The seed for 'random_generator_'.
        std::mt19937::result_type random_seed_;
        std::mt19937 random_generator_;

        friend class cereal::access;
        template<class Archive>
        void save(Archive& ar) const
            {
                ar(cereal::make_nvp("block_size", block_size_),
                   cereal::make_nvp("ColorGenerator", get_generator_wrapper()->unwrap()));
            }
        template<class Archive>
        void load(Archive& ar)
            {
                std::shared_ptr<ColorGenerator> generator;
                ar(cereal::make_nvp("block_size", block_size_));
                ar(cereal::make_nvp("ColorGenerator", generator));
                set_generator_wrapper(std::make_shared<ColorGeneratorWrapper>(generator));
            }
    };
}

CEREAL_REGISTER_TYPE_WITH_NAME(colors::VertexPainterRandom, "VertexPainterRandom");
CEREAL_REGISTER_POLYMORPHIC_RELATION(colors::VertexPainter, colors::VertexPainterRandom)

#endif // VERTEX_PAINTER_RANDOM_H
