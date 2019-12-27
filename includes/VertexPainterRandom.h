#ifndef VERTEX_PAINTER_RANDOM_H
#define VERTEX_PAINTER_RANDOM_H


#include "VertexPainter.h"
#include "helper_math.h"
#include "ColorsGeneratorSerializer.h"
#include "LoadMenu.h"

namespace colors
{
    class VertexPainterRandom : public VertexPainter
    {
    public:
        VertexPainterRandom(); // Create a default generator
        explicit VertexPainterRandom(const ColorGeneratorWrapper& wrapper);
        virtual ~VertexPainterRandom() {}
        // This class is mainly used polymorphic-ally, so deleting these
        // constructors saved some LoC so potential bugs.
        VertexPainterRandom(const VertexPainterRandom& other) = delete;
        VertexPainterRandom(VertexPainterRandom&& other) = delete;
        VertexPainterRandom& operator=(const VertexPainterRandom& other) = delete;
        VertexPainterRandom& operator=(VertexPainterRandom&& other) = delete;

        // Generate a new seed.
        void randomize();

        // Getter/Setter
        int get_block_size() const;
        void set_block_size(int block_size);

        // Paint 'vertices' according to a random real number.
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

        // The number of consecutive vertices to paint the same color.
        // Invariant: must be strictly positive
        int block_size_;
        // The seed for 'random_generator_'.
        std::mt19937::result_type random_seed_;
        std::mt19937 random_generator_;

        friend class cereal::access;
        template<class Archive>
        void save(Archive& ar, const u32) const
            {
                // ar(cereal::make_nvp("block_size", block_size_),
                //    cereal::make_nvp("ColorGenerator", get_generator_wrapper()->unwrap()));
                ar(cereal::make_nvp("block_size", block_size_));

                auto color_generator = get_generator_wrapper().unwrap();
                auto serializer = ColorGeneratorSerializer(color_generator);
                ar(cereal::make_nvp("ColorGenerator", serializer));
            }
        template<class Archive>
        void load(Archive& ar, const u32)
            {
                ar(cereal::make_nvp("block_size", block_size_));

                if (block_size_ < 1)
                {
                    block_size_ = 1;
                    controller::LoadMenu::add_loading_error_message("VertexPainterRandom's block_size was smaller than 1, so it is set to 1.");
                }

                ColorGeneratorSerializer serializer;
                ar(cereal::make_nvp("ColorGenerator", serializer));
                generator_ = ColorGeneratorWrapper(serializer.get_serialized());
            }
    };
}


#endif // VERTEX_PAINTER_RANDOM_H
