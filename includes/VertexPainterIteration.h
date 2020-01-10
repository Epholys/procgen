#ifndef VERTEX_PAINTER_ITERATION_H
#define VERTEX_PAINTER_ITERATION_H


#include "ColorsGeneratorSerializer.h"
#include "VertexPainter.h"

namespace colors
{
class VertexPainterIteration : public VertexPainter
{
  public:
    VertexPainterIteration() = default;
    explicit VertexPainterIteration(const ColorGeneratorWrapper& wrapper);
    virtual ~VertexPainterIteration()
    {
    }
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
                                const std::vector<u8>& vertices_iteration,
                                const std::vector<bool>& transparent,
                                int max_iteration,
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
        auto color_generator = get_generator_wrapper().unwrap();
        auto serializer = ColorGeneratorSerializer(color_generator);
        ar(cereal::make_nvp("ColorGenerator", serializer));
    }
    template<class Archive>
    void load(Archive& ar, const u32)
    {
        ColorGeneratorSerializer serializer;
        ar(cereal::make_nvp("ColorGenerator", serializer));
        generator_ = ColorGeneratorWrapper(serializer.get_serialized());
    }
};
} // namespace colors

#endif // VERTEX_PAINTER_ITERATION_H
