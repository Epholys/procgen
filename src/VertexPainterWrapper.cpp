#include "VertexPainterWrapper.h"

#include "VertexPainterConstant.h"

#include <utility>

namespace colors
{
VertexPainterWrapper::VertexPainterWrapper()
    : painter_ {std::make_shared<VertexPainterConstant>()}
{
}

VertexPainterWrapper::VertexPainterWrapper(std::shared_ptr<VertexPainter> painter)
    : painter_ {std::move(painter)}
{
}

VertexPainterWrapper::VertexPainterWrapper(const VertexPainterWrapper& other)
    : Indicator(other)
    , painter_ {other.painter_->clone()}
{
}

VertexPainterWrapper::VertexPainterWrapper(VertexPainterWrapper&& other) noexcept
    : painter_ {std::move(other.painter_)}
{
    other.painter_.reset();
}

VertexPainterWrapper& VertexPainterWrapper::operator=(const VertexPainterWrapper& other)
{
    if (this != &other)
    {
        painter_ = other.painter_->clone();
    }
    return *this;
}

VertexPainterWrapper& VertexPainterWrapper::operator=(VertexPainterWrapper&& other) noexcept
{
    if (this != &other)
    {
        painter_ = std::move(other.painter_);
        other.painter_.reset();
    }
    return *this;
}

std::shared_ptr<VertexPainter> VertexPainterWrapper::unwrap() const
{
    return painter_;
}

void VertexPainterWrapper::wrap(std::shared_ptr<VertexPainter> painter)
{
    painter_ = std::move(painter);
    indicate_modification();
}

bool VertexPainterWrapper::poll_modification()
{
    return Indicator::poll_modification() || painter_->poll_modification();
}
} // namespace colors
