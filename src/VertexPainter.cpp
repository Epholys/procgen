#include "VertexPainter.h"
#include "procgui.h"
#include "geometry.h"

namespace colors
{
    VertexPainter::VertexPainter()
        : Observable{}
        , OGenBuff{std::make_shared<ColorGeneratorBuffer>()}
    {
        add_callback([this](){notify();});
    }

    VertexPainter::VertexPainter(const std::shared_ptr<ColorGenerator> gen)
        : Observable{}
        , OGenBuff{std::make_shared<ColorGeneratorBuffer>(gen)}
    {
        add_callback([this](){notify();});
    }

    VertexPainter::VertexPainter(const VertexPainter& other)
        : Observable{}
        , OGenBuff{other.get_target()}
    {
        add_callback([this](){notify();});
    }

    VertexPainter::VertexPainter(VertexPainter&& other)
        : Observable{}
        , OGenBuff{std::move(other.get_target())}
    {
        add_callback([this](){notify();});
        
        other.set_target(nullptr);
    }
    
    VertexPainter& VertexPainter::operator=(const VertexPainter& other)
    {
        if (this != &other)
        {
            set_target(other.get_target());

            add_callback([this](){notify();});
        }
        return *this;
    }

    VertexPainter& VertexPainter::operator=(VertexPainter&& other)
    {
        if (this != &other)
        {
            set_target(std::move(other.get_target()));

            add_callback([this](){notify();});
            
            other.set_target(nullptr);
        }
        return *this;
    }

    std::shared_ptr<VertexPainter> VertexPainter::clone() const
    {
        return clone_impl();
    }
    
    std::shared_ptr<ColorGeneratorBuffer> VertexPainter::get_generator_buffer() const
    {
        return get_target();
    }
}
