#ifndef VERTEX_PAINTER_RADIAL_H
#define VERTEX_PAINTER_RADIAL_H


#include "VertexPainter.h"
#include "helper_cereal.hpp"

namespace colors
{
    class VertexPainterRadial : public VertexPainter
    {
    public:
        VertexPainterRadial(); // Create a default generator
        explicit VertexPainterRadial(const std::shared_ptr<ColorGeneratorWrapper> wrapper);
        virtual ~VertexPainterRadial() {}
        // This class is mainly used polymorphic-ally, so deleting these
        // constructors saved some LoC so potential bugs.
        VertexPainterRadial(const VertexPainterRadial& other) = delete;
        VertexPainterRadial(VertexPainterRadial&& other) = delete;
        VertexPainterRadial& operator=(const VertexPainterRadial& other) = delete;
        VertexPainterRadial& operator=(VertexPainterRadial&& other) = delete;
        
        // Getters
        sf::Vector2f get_center() const;

        // Setter
        void set_center(sf::Vector2f center);
        
        // Paint 'vertices' in a 'center_' centered distance-bases radial
        // fashion with the informations of 'bounding_box' according to the rule
        // with the colors from the ColorGenerator.
        // 'iteration_of_vertices' and 'max_recursion' are not used.
        virtual void paint_vertices(std::vector<sf::Vertex>& vertices,
                                    const std::vector<int>& iteration_of_vertices,
                                    int max_recursion,
                                    sf::FloatRect bounding_box) override;

        // Implements the deep-copy cloning.
        virtual std::shared_ptr<VertexPainter> clone() const override;

    private:
        // [0, 1] center: 0 is the left/up of the bounding_box and 1 the right/down.
        sf::Vector2f center_ {0, 0};
        
        friend class cereal::access;
        template<class Archive>
        void save(Archive& ar, const std::uint32_t) const
            {
                ar(cereal::make_nvp("center", center_),
                   cereal::make_nvp("ColorGenerator", get_generator_wrapper()->unwrap()));
            }
        template<class Archive>
        void load(Archive& ar, const std::uint32_t)
            {
                std::shared_ptr<ColorGenerator> generator;
                ar(cereal::make_nvp("center", center_));
                ar(cereal::make_nvp("ColorGenerator", generator));
                set_generator_wrapper(std::make_shared<ColorGeneratorWrapper>(generator));
            }
    };
}


#endif // VERTEX_PAINTER_RADIAL_H
