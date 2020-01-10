#ifndef VERTEX_PAINTER_WRAPPER
#define VERTEX_PAINTER_WRAPPER


#include "VertexPainter.h"

namespace colors
{
// VertexPainterWrapper observes a VertexPainter for the GUI: if a new
// painter is created (chaning for example from a Linear to a Radial), it
// will notify() all Observers. It will also notify the Observers if the
// VertexPainter is modified.
class VertexPainterWrapper : public Indicator
{
  public:
    // Default constructor, construct an empy VertexPainter.
    VertexPainterWrapper();
    explicit VertexPainterWrapper(std::shared_ptr<VertexPainter> painter);
    virtual ~VertexPainterWrapper()
    {
    }
    // Rule-of-five deep copy.
    VertexPainterWrapper(const VertexPainterWrapper& other);
    VertexPainterWrapper(VertexPainterWrapper&& other) noexcept;
    VertexPainterWrapper& operator=(const VertexPainterWrapper& other);
    VertexPainterWrapper& operator=(VertexPainterWrapper&& other) noexcept;

    // Getter
    std::shared_ptr<VertexPainter> unwrap() const;

    // Setter
    void wrap(std::shared_ptr<VertexPainter> painter);

    virtual bool poll_modification() override;

  private:
    std::shared_ptr<VertexPainter> painter_;
};
} // namespace colors


#endif // VERTEX_PAINTER_WRAPPER
