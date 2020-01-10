#include "VertexPainter.h"

#include <utility>

namespace colors
{
VertexPainter::VertexPainter(ColorGeneratorWrapper wrapper)
    : generator_ {std::move(wrapper)}
{
}

const ColorGeneratorWrapper& VertexPainter::get_generator_wrapper() const
{
    return generator_;
}
ColorGeneratorWrapper& VertexPainter::ref_generator_wrapper()
{
    return generator_;
}
void VertexPainter::set_generator_wrapper(const ColorGeneratorWrapper& color_generator_wrapper)
{
    generator_ = color_generator_wrapper;
    indicate_modification();
}

void VertexPainter::supplementary_drawing(sf::FloatRect /*unused*/) const
{
}

bool VertexPainter::poll_modification()
{
    return Indicator::poll_modification() || generator_.poll_modification();
}
} // namespace colors
