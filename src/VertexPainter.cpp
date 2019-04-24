#include "VertexPainter.h"

namespace colors
{
    VertexPainter::VertexPainter()
        : Observable{}
        , OGenBuff{std::make_shared<ColorGeneratorWrapper>()}
    {
        add_callback([this](){notify();});
    }

    VertexPainter::VertexPainter(const std::shared_ptr<ColorGeneratorWrapper> wrapper)
        : Observable{}
        , OGenBuff{wrapper}
    {
        add_callback([this](){notify();});
    }

    // VertexPainter::VertexPainter(const VertexPainter& other)
    //     : Observable{}
    //     , OGenBuff{std::make_shared<ColorGeneratorWrapper>(*other.get_target())}
    // {
    //     add_callback([this](){notify();});
    // }

    // VertexPainter::VertexPainter(VertexPainter&& other)
    //     : Observable{}
    //     , OGenBuff{std::move(other.get_target())}
    // {
    //     add_callback([this](){notify();});
        
    //     other.set_target(nullptr);
    // }
    
    // VertexPainter& VertexPainter::operator=(const VertexPainter& other)
    // {
    //     if (this != &other)
    //     {
    //         set_target(std::make_shared<ColorGeneratorWrapper>(*other.get_target()));

    //         add_callback([this](){notify();});
    //     }
    //     return *this;
    // }

    // VertexPainter& VertexPainter::operator=(VertexPainter&& other)
    // {
    //     if (this != &other)
    //     {
    //         set_target(std::move(other.get_target()));

    //         add_callback([this](){notify();});
            
    //         other.set_target(nullptr);
    //     }
    //     return *this;
    // }

    std::shared_ptr<VertexPainter> VertexPainter::clone() const
    {
        return clone_impl();
    }
    
    std::shared_ptr<ColorGeneratorWrapper> VertexPainter::get_generator_wrapper() const
    {
        return get_target();
    }
    void VertexPainter::set_generator_wrapper(std::shared_ptr<ColorGeneratorWrapper> color_generator_wrapper)
    {
        OGenBuff::set_target(color_generator_wrapper);
        add_callback([this](){notify();});
        notify();
    }
    void VertexPainter::set_target(std::shared_ptr<ColorGeneratorWrapper> color_generator_wrapper)
    {
        set_generator_wrapper(color_generator_wrapper);
    }

}
