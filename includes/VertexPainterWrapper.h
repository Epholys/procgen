#ifndef VERTEX_PAINTER_WRAPPER
#define VERTEX_PAINTER_WRAPPER


#include "VertexPainter.h"
#include "VertexPainterLinear.h"

namespace colors
{
    // VertexPainterWrapper observes a VertexPainter for the GUI: if a new
    // painter is created (chaning for example from a Linear to a Radial), it
    // will notify() all Observers. It will also notify the Observers if the
    // VertexPainter is modified.
    class VertexPainterWrapper : public Observable
                              , public Observer<VertexPainter>
    {
    public:
        using OPainter = Observer<VertexPainter>;
        
        // Default constructor, construct an empy VertexPainter.
        VertexPainterWrapper();
        explicit VertexPainterWrapper(std::shared_ptr<VertexPainter> painter);
        // Rule-of-five shallow copy.
        VertexPainterWrapper(const VertexPainterWrapper& other);
        VertexPainterWrapper(VertexPainterWrapper&& other);
        VertexPainterWrapper& operator=(const VertexPainterWrapper& other);
        VertexPainterWrapper& operator=(VertexPainterWrapper&& other);

        VertexPainterWrapper clone() const;

        // Getter
        std::shared_ptr<VertexPainter> unwrap() const;

        // Setter
        void wrap(std::shared_ptr<VertexPainter> painter);
    };
}


#endif // VERTEX_PAINTER_WRAPPER
