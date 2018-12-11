#include "VertexPainterBuffer.h"
#include "VertexPainterConstant.h"

namespace colors
{
    VertexPainterBuffer::VertexPainterBuffer()
        : Observable{}
        , OPainter{std::make_shared<VertexPainterConstant>()}
    {
        add_callback([this](){notify();});
    }

    VertexPainterBuffer::VertexPainterBuffer(std::shared_ptr<VertexPainter> painter)
        : OPainter{painter}
    {
        add_callback([this](){notify();});
    }
    
    VertexPainterBuffer::VertexPainterBuffer(const VertexPainterBuffer& other)
        : Observable{}
        , OPainter{other.get_target()}
    {
        add_callback([this](){notify();});
    }
    
    VertexPainterBuffer::VertexPainterBuffer(VertexPainterBuffer&& other)
        : Observable{}
        , OPainter{std::move(other.get_target())}
    {
        add_callback([this](){notify();});
        other.set_target(nullptr);
    }
    
    VertexPainterBuffer& VertexPainterBuffer::operator=(const VertexPainterBuffer& other)
    {
        if (this != &other)
        {
            set_target(other.get_target());
            add_callback([this](){notify();});
        }
        return *this;
    }
    
    VertexPainterBuffer& VertexPainterBuffer::operator=(VertexPainterBuffer&& other)
    {
        if (this != &other)
        {
            set_target(std::move(other.get_target()));
            add_callback([this](){notify();});
            other.set_target(nullptr);
        }
        return *this;
    }

    VertexPainterBuffer VertexPainterBuffer::clone() const
    {
        return VertexPainterBuffer(get_target()->clone());
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
