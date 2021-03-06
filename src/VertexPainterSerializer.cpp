#include "VertexPainterSerializer.h"

#include <utility>

namespace colors
{
VertexPainterSerializer::VertexPainterSerializer()
    : serialized_ {nullptr}
{
}

VertexPainterSerializer::VertexPainterSerializer(std::shared_ptr<VertexPainter> to_serialize)
    : serialized_ {std::move(to_serialize)}
{
    Expects(serialized_);
}

std::shared_ptr<VertexPainter> VertexPainterSerializer::get_serialized() const
{
    Expects(serialized_);
    return serialized_;
}
} // namespace colors
