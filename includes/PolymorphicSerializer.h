#ifndef POLYMORPHIC_SERIALIZER_H
#define POLYMORPHIC_SERIALIZER_H


#include <memory>
#include <map>
#include <string>
#include <functional>

#include "helper_cereal.hpp"

#include "ColorsGenerator.h"
#include "VertexPainterComposite.h"

namespace colors
{
    template<class Archive>
    class ColorGeneratorSerializer
    {
    public:
        ColorGeneratorSerializer()
            : serialized_ {nullptr}
            {
            }                
        explicit ColorGeneratorSerializer(std::shared_ptr<ColorGenerator> to_serialize)
            : serialized_ {to_serialize}
            {
                Expects(serialized_);
            }                
       

        std::shared_ptr<ColorGenerator> get_serialized() const
            {
                Expects(serialized_);
                return serialized_;
            }
    private:
        friend class cereal::access;
        void save(Archive& ar, const std::uint32_t) const
            {
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
                SERIALIZE_COLORGEN_CHILD(impl::ColorGeneratorComposite);
            }
    
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
                DESERIALIZE_COLORGEN_CHILD(impl::ColorGeneratorComposite);
            }

        std::shared_ptr<ColorGenerator> serialized_;
    };
}

#endif // POLYMORPHIC_SERIALIZER_H
