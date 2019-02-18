#ifndef VERTEX_PAINTER_LINEAR_H
#define VERTEX_PAINTER_LINEAR_H

#include "VertexPainter.h"

namespace cereal
{
    template <class Archive, class N,
              traits::EnableIf<traits::is_text_archive<Archive>::value> = traits::sfinae> inline
    void serialize(Archive& ar, sf::Vector2<N>& vec)
    {
        ar(cereal::make_nvp("x", vec.x),
           cereal::make_nvp("y", vec.y));
    }
}

namespace colors
{
    // Paint the vertices with a ColorGenerator following a line passing through
    // the center of the bounding box at a certain 'angle_'.
    class VertexPainterLinear : public VertexPainter
    {
    public:
        VertexPainterLinear(); // Create a default generator
        explicit VertexPainterLinear(const std::shared_ptr<ColorGenerator> gen);
        // Shallow rule-of-five constructors.
        VertexPainterLinear(const VertexPainterLinear& other);
        VertexPainterLinear(VertexPainterLinear&& other);
        VertexPainterLinear& operator=(const VertexPainterLinear& other);
        VertexPainterLinear& operator=(VertexPainterLinear&& other);
        
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

    private:
        // Implements the deep-copy cloning.
        virtual std::shared_ptr<VertexPainter> clone_impl() const override;

        friend class cereal::access;
        template<class Archive>
        void save(Archive& ar) const
            {
                ar(cereal::make_nvp("angle", angle_),
                   cereal::make_nvp("center", center_),
                   cereal::make_nvp("ColorGenerator", get_generator_wrapper()->unwrap()));
            }
        template<class Archive>
        void load(Archive& ar)
            {
                std::shared_ptr<ColorGenerator> generator;
                ar(cereal::make_nvp("angle", angle_));
                ar(cereal::make_nvp("center", center_));
                ar(cereal::make_nvp("ColorGenerator", generator));
                set_generator_wrapper(std::make_shared<ColorGeneratorWrapper>(generator));
            }
        
        
        float angle_ {0};
        sf::Vector2f center_ {0.5,0.5};
    };
}

CEREAL_REGISTER_TYPE_WITH_NAME(colors::VertexPainterLinear, "VertexPainterLinear");
CEREAL_REGISTER_POLYMORPHIC_RELATION(colors::VertexPainter, colors::VertexPainterLinear)


#endif // VERTEX_PAINTER_LINEAR_H
