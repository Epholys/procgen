#ifndef VERTEX_PAINTER_BUFFER
#define VERTEX_PAINTER_BUFFER


#include "VertexPainter.h"

namespace colors
{
    // VertexPainterBuffer observes a VertexPainter for the GUI: if a new
    // painter is created (chaning for example from a Linear to a Radial), it
    // will notify() all Observers. It will also notify the Observers if the
    // VertexPainter is modified.
    class VertexPainterBuffer : public Observable
                              , public Observer<VertexPainter>
    {
    public:
        // Default constructor, construct an empy VertexPainter.
        VertexPainterBuffer();
        explicit VertexPainterBuffer(std::shared_ptr<VertexPainter> painter);
        // Rule-of-five shallow copy.
        VertexPainterBuffer(const VertexPainterBuffer& other);
        VertexPainterBuffer(VertexPainterBuffer&& other);
        VertexPainterBuffer& operator=(const VertexPainterBuffer& other);
        VertexPainterBuffer& operator=(VertexPainterBuffer&& other);

        // Getter
        std::shared_ptr<VertexPainter> get_painter() const;

        // Setter
        void set_painter(std::shared_ptr<VertexPainter> painter);
    };
}


#endif // VERTEX_PAINTER_BUFFER
