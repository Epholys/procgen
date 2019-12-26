#include "VertexPainter.h"

namespace colors
{
    VertexPainter::VertexPainter()
        : generator_{std::make_shared<ColorGeneratorWrapper>()}
    {
    }

    VertexPainter::VertexPainter(const std::shared_ptr<ColorGeneratorWrapper> wrapper)
        : generator_{wrapper}
    {
    }

    std::shared_ptr<const ColorGeneratorWrapper> VertexPainter::get_generator_wrapper() const
    {
        return generator_;
    }
    std::shared_ptr<ColorGeneratorWrapper> VertexPainter::ref_generator_wrapper() const
    {
        return generator_;
    }
    void VertexPainter::set_generator_wrapper(std::shared_ptr<ColorGeneratorWrapper> color_generator_wrapper)
    {
        generator_ = color_generator_wrapper;
        indicate_modification();
    }

    void VertexPainter::supplementary_drawing(sf::FloatRect) const
    {
    }

    bool VertexPainter::poll_modification()
    {
        return Indicator::poll_modification() | generator_->poll_modification();
    }
}
