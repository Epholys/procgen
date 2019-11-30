#ifndef COLOR_GENERATOR_SERIALIZER_H
#define COLOR_GENERATOR_SERIALIZER_H


#include <memory>
#include <map>
#include <string>
#include <functional>

#include "types.h"
#include "helper_cereal.hpp"
#include "ColorsGenerator.h"
#include "VertexPainterComposite.h"

namespace colors
{
    using namespace colors::impl;

    // Helper class to serialize polymorphic classes ColorGenerators.
    // cereal can handle the case of serializing and deserializing polymorphic
    // calsses. However, the end result is not "pretty", meaning that generating
    // save files becomes complicated. As such, this class implements (in a dirty way)
    // the following format:
    //
    // ColorGenerator": {
    //     "cereal_class_version": 0,
    //     "type": "ConstantColor",
    //     "color": "#FFFFFFFF"
    // }
    //
    // Where 'type' is the type of the ColorGenerator.
    // This is not very pretty: each class must have a 'type_name()' method that
    // returns its class name. With this, it is possible to generator code with
    // macros, who automatically add to "type" the corresponding class name.
    //
    // The usage of this class is simple:
    // - to serialize, construct a Serializer with a shared_ptr of the
    //   ColorGenerator and save it.
    // - to deserialize, construct a empty serializer, load it and call
    //   'get_serialized()' to obtain the deserialized ColorGenerator
    class ColorGeneratorSerializer
    {
    public:
        // Create a ColorGeneratorSerializer with a nullptr.
        ColorGeneratorSerializer();
        // Create a ColorGeneratorSerializer with 'to_serialize'.
        explicit ColorGeneratorSerializer(std::shared_ptr<ColorGenerator> to_serialize);
        ~ColorGeneratorSerializer() = default;
        ColorGeneratorSerializer(const ColorGeneratorSerializer& other) = default;
        ColorGeneratorSerializer(ColorGeneratorSerializer&& other) = default;
        ColorGeneratorSerializer& operator=(const ColorGeneratorSerializer& other) = default;
        ColorGeneratorSerializer& operator=(ColorGeneratorSerializer&& other) = default;

        // Getter
        std::shared_ptr<ColorGenerator> get_serialized() const;

    private:
        friend class cereal::access;
        template<class Archive>
        void save(Archive& ar, const u32) const
            {
                Expects(serialized_);

                u32 version = 0; // ignored

                std::string type = serialized_->type_name();
                ar(cereal::make_nvp("type", type));

#define SERIALIZE_COLORGEN_CHILD(Child)                                 \
                do {                                                    \
                    if (type == #Child)                                 \
                    {                                                   \
                        auto child_ptr = std::dynamic_pointer_cast<Child>(serialized_); \
                        Expects(child_ptr);                             \
                        child_ptr->save(ar, version);                   \
                        return;                                         \
                    }                                                   \
                }                                                       \
                while(false)

                SERIALIZE_COLORGEN_CHILD(ConstantColor);
                SERIALIZE_COLORGEN_CHILD(LinearGradient);
                SERIALIZE_COLORGEN_CHILD(DiscreteGradient);
                SERIALIZE_COLORGEN_CHILD(ColorGeneratorComposite);
#undef SERIALIZE_COLORGEN_CHILD
            }

        template<class Archive>
        void load(Archive& ar, const u32)
            {
                u32 version = 0; // ignored

                std::string type;
                ar(cereal::make_nvp("type", type));

#define DESERIALIZE_COLORGEN_CHILD(Child)                               \
                do {                                                    \
                    if (type == #Child)                                 \
                    {                                                   \
                        Child* ptr = new Child();                       \
                        ptr->load(ar, version);                         \
                        serialized_ = std::shared_ptr<ColorGenerator>(ptr); \
                    }                                                   \
                }                                                       \
                while(false)

                DESERIALIZE_COLORGEN_CHILD(ConstantColor);
                DESERIALIZE_COLORGEN_CHILD(LinearGradient);
                DESERIALIZE_COLORGEN_CHILD(DiscreteGradient);
                DESERIALIZE_COLORGEN_CHILD(ColorGeneratorComposite);
                Expects(serialized_);
#undef DESERIALIZE_COLORGEN_CHILD
            }

        // The ColorGenerator to serialize and deserialize.
        std::shared_ptr<ColorGenerator> serialized_ {nullptr};
    };
}

#endif // COLOR_GENERATOR_SERIALIZER_H
