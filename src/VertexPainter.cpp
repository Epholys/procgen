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

    void VertexPainter::supplementary_drawing(sf::FloatRect) const
    {
    }
}
