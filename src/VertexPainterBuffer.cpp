#include "VertexPainterBuffer.h"

namespace colors
{
    VertexPainterBuffer::VertexPainterBuffer()
        : Observable{}
        , Observer<VertexPainter>{std::make_shared<VertexPainter>()}
    {
        add_callback([this](){notify();});
    }
    
    VertexPainterBuffer::VertexPainterBuffer(const VertexPainterBuffer& other)
        : Observable{}
        , Observer<VertexPainter>{other.get_target()}
    {
        add_callback([this](){notify();});
    }
    
    VertexPainterBuffer::VertexPainterBuffer(VertexPainterBuffer&& other)
        : Observable{}
        , Observer<VertexPainter>{std::move(other.get_target())}
    {
        add_callback([this](){notify();});
        other.set_target(nullptr);
    }
    
    VertexPainterBuffer& VertexPainterBuffer::operator=(const VertexPainterBuffer& other)
    {
        if (this != &other)
        {
            Observer<VertexPainter> {other.get_target()};
            add_callback([this](){notify();});
        }
        return *this;
    }
    
    VertexPainterBuffer& VertexPainterBuffer::operator=(VertexPainterBuffer&& other)
    {
        if (this != &other)
        {
            Observer<VertexPainter> {std::move(other.get_target())};
            add_callback([this](){notify();});
            other.set_target(nullptr);
        }
        return *this;
    }

    std::shared_ptr<VertexPainter> VertexPainterBuffer::get_painter() const
    {
        return get_target();
    }

    void VertexPainterBuffer::set_painter(std::shared_ptr<VertexPainter> painter)
    {
        set_target(painter);
        add_callback([this](){notify();});
        notify();
    }
}
