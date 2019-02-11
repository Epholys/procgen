#include "VertexPainterWrapper.h"
#include "VertexPainterConstant.h"

namespace colors
{
    VertexPainterWrapper::VertexPainterWrapper()
        : Observable{}
        , OPainter{std::make_shared<VertexPainterConstant>()}
    {
        add_callback([this](){notify();});
    }

    VertexPainterWrapper::VertexPainterWrapper(std::shared_ptr<VertexPainter> painter)
        : OPainter{painter}
    {
        add_callback([this](){notify();});
    }
    
    VertexPainterWrapper::VertexPainterWrapper(const VertexPainterWrapper& other)
        : Observable{}
        , OPainter{other.get_target()}
    {
        add_callback([this](){notify();});
    }
    
    VertexPainterWrapper::VertexPainterWrapper(VertexPainterWrapper&& other)
        : Observable{}
        , OPainter{std::move(other.get_target())}
    {
        add_callback([this](){notify();});
        other.set_target(nullptr);
    }
    
    VertexPainterWrapper& VertexPainterWrapper::operator=(const VertexPainterWrapper& other)
    {
        if (this != &other)
        {
            set_target(other.get_target());
            add_callback([this](){notify();});
        }
        return *this;
    }
    
    VertexPainterWrapper& VertexPainterWrapper::operator=(VertexPainterWrapper&& other)
    {
        if (this != &other)
        {
            set_target(std::move(other.get_target()));
            add_callback([this](){notify();});
            other.set_target(nullptr);
        }
        return *this;
    }

    VertexPainterWrapper VertexPainterWrapper::clone() const
    {
        return VertexPainterWrapper(get_target()->clone());
    }
    
    std::shared_ptr<VertexPainter> VertexPainterWrapper::unwrap() const
    {
        return get_target();
    }

    void VertexPainterWrapper::wrap(std::shared_ptr<VertexPainter> painter)
    {
        set_target(painter);
        add_callback([this](){notify();});
        notify();
    }
}
