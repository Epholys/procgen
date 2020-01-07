#include "ColorsGeneratorSerializer.h"

#include <utility>

namespace colors
{
ColorGeneratorSerializer::ColorGeneratorSerializer()
    : serialized_ {nullptr}
{
}

ColorGeneratorSerializer::ColorGeneratorSerializer(std::shared_ptr<ColorGenerator> to_serialize)
    : serialized_ {std::move(to_serialize)}
{
    Expects(serialized_);
}

std::shared_ptr<ColorGenerator> ColorGeneratorSerializer::get_serialized() const
{
    Expects(serialized_);
    return serialized_;
}
} // namespace colors
