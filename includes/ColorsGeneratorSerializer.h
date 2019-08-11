#ifndef COLOR_GENERATOR_SERIALIZER_H
#define COLOR_GENERATOR_SERIALIZER_H


#include <memory>
#include <map>
#include <string>
#include <functional>

#include "helper_cereal.hpp"

#include "ColorsGenerator.h"
#include "VertexPainterComposite.h"

namespace colors
{
    using namespace colors::impl;
    
    class ColorGeneratorSerializer
    {
    public:
        ColorGeneratorSerializer();
        explicit ColorGeneratorSerializer(std::shared_ptr<ColorGenerator> to_serialize);

        std::shared_ptr<ColorGenerator> get_serialized() const;
        
    private:
        friend class cereal::access;
        template<class Archive>
        void save(Archive& ar, const std::uint32_t) const
            {
                Expects(serialized_);

                std::uint32_t version = 0; // ignored
                
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
        void load(Archive& ar, const std::uint32_t)
            {
                std::uint32_t version = 0; // ignored
            
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

        std::shared_ptr<ColorGenerator> serialized_;
    };
}

#endif // COLOR_GENERATOR_SERIALIZER_H
