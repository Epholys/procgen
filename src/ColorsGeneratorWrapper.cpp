#include "ColorsGeneratorWrapper.h"

#include <utility>

namespace colors
{
ColorGeneratorWrapper::ColorGeneratorWrapper()
    : generator_ {std::make_shared<ConstantColor>()}
{
}

ColorGeneratorWrapper::ColorGeneratorWrapper(std::shared_ptr<ColorGenerator> gen)
    : generator_ {std::move(gen)}
{
}

ColorGeneratorWrapper::ColorGeneratorWrapper(const ColorGeneratorWrapper& other)
    : Indicator(other)
    , generator_ {other.generator_->clone()}
{
}
ColorGeneratorWrapper::ColorGeneratorWrapper(ColorGeneratorWrapper&& other) noexcept
    : generator_ {std::move(other.generator_)}
{
    other.generator_.reset();
}
ColorGeneratorWrapper& ColorGeneratorWrapper::operator=(const ColorGeneratorWrapper& other)
{
    if (this != &other)
    {
        generator_ = other.generator_->clone();
    }
    return *this;
}
ColorGeneratorWrapper& ColorGeneratorWrapper::operator=(ColorGeneratorWrapper&& other) noexcept
{
    if (this != &other)
    {
        generator_ = std::move(other.generator_);
        other.generator_.reset();
    }
    return *this;
}

std::shared_ptr<ColorGenerator> ColorGeneratorWrapper::unwrap() const
{
    return generator_;
}

void ColorGeneratorWrapper::wrap(std::shared_ptr<ColorGenerator> gen)
{
    generator_ = std::move(gen);
    indicate_modification();
}

bool ColorGeneratorWrapper::poll_modification()
{
    return Indicator::poll_modification() || generator_->poll_modification();
}
} // namespace colors
