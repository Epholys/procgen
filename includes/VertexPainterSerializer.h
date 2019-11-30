#ifndef VERTEX_PAINTER_SERIALIZER_H
#define VERTEX_PAINTER_SERIALIZER_H


#include <memory>
#include <map>
#include <string>
#include <functional>

#include "helper_cereal.hpp"

#include "VertexPainter.h"
#include "VertexPainterConstant.h"
#include "VertexPainterIteration.h"
#include "VertexPainterLinear.h"
#include "VertexPainterRadial.h"
#include "VertexPainterRandom.h"
#include "VertexPainterSequential.h"
#include "VertexPainterComposite.h"

namespace colors
{
    class VertexPainterComposite;
    
    // Helper class to serialize polymorphic classes VertexPainters.
    // cereal can handle the case of serializing and deserializing polymorphic
    // calsses. However, the end result is not "pretty", meaning that generating
    // save files becomes complicated. As such, this class implements (in a dirty way)
    // the following format:
    // 
    // VertexPainter": {
    //     "cereal_class_version": 0,
    //     "type": "ConstantColor",
    //     "color": "#FFFFFFFF"
    // }
    //
    // Where 'type' is the type of the VertexPainter.
    // This is not very pretty: each class must have a 'type_name()' method that
    // returns its class name. With this, it is possible to generator code with
    // macros, who automatically add to "type" the corresponding class name.
    //
    // The usage of this class is simple:
    // - to serialize, construct a Serializer with a shared_ptr of the
    //   VertexPainter and save it.
    // - to deserialize, construct a empty serializer, load it and call
    //   'get_serialized()' to obtain the deserialized VertexPainter
    class VertexPainterSerializer
    {
    public:
        // Create a VertexPainterSerializer with a nullptr.
        VertexPainterSerializer();
        // Create a VertexPainterSerializer with 'to_serialize'.
        explicit VertexPainterSerializer(std::shared_ptr<VertexPainter> to_serialize);
        ~VertexPainterSerializer() = default;
        VertexPainterSerializer(const VertexPainterSerializer& other) = default;
        VertexPainterSerializer(VertexPainterSerializer&& other) = default;
        VertexPainterSerializer& operator=(const VertexPainterSerializer& other) = default;
        VertexPainterSerializer& operator=(VertexPainterSerializer&& other) = default;
        
        // Getter
        std::shared_ptr<VertexPainter> get_serialized() const;
        
    private:
        friend class cereal::access;
        template<class Archive>
        void save(Archive& ar, const u32) const
            {
                Expects(serialized_);

                u32 version = 0; // ignored
                
                std::string type = serialized_->type_name();
                ar(cereal::make_nvp("type", type));

#define SERIALIZE_PAINTER_CHILD(Child)                                 \
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

                SERIALIZE_PAINTER_CHILD(VertexPainterConstant);
                SERIALIZE_PAINTER_CHILD(VertexPainterIteration);
                SERIALIZE_PAINTER_CHILD(VertexPainterLinear);
                SERIALIZE_PAINTER_CHILD(VertexPainterRadial);
                SERIALIZE_PAINTER_CHILD(VertexPainterRandom);
                SERIALIZE_PAINTER_CHILD(VertexPainterSequential);

                SERIALIZE_PAINTER_CHILD(VertexPainterComposite);
#undef SERIALIZE_PAINTER_CHILD
            }
    
        template<class Archive>
        void load(Archive& ar, const u32)
            {
                u32 version = 0; // ignored

                std::string type;
                ar(cereal::make_nvp("type", type));
            
#define DESERIALIZE_PAINTER_CHILD(Child)                               \
                do {                                                    \
                    if (type == #Child)                                 \
                    {                                                   \
                        Child* ptr = new Child();                       \
                        ptr->load(ar, version);                         \
                        serialized_ = std::shared_ptr<VertexPainter>(ptr); \
                    }                                                   \
                }                                                       \
                while(false)
            
                DESERIALIZE_PAINTER_CHILD(VertexPainterConstant);
                DESERIALIZE_PAINTER_CHILD(VertexPainterIteration);
                DESERIALIZE_PAINTER_CHILD(VertexPainterLinear);
                DESERIALIZE_PAINTER_CHILD(VertexPainterRadial);
                DESERIALIZE_PAINTER_CHILD(VertexPainterRandom);
                DESERIALIZE_PAINTER_CHILD(VertexPainterSequential);

                DESERIALIZE_PAINTER_CHILD(VertexPainterComposite);
                Expects(serialized_);
#undef DESERIALIZE_PAINTER_CHILD
            }

        std::shared_ptr<VertexPainter> serialized_;
    };
}

#endif // VERTEX_PAINTER_SERIALIZER_H
