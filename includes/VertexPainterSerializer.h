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
    
    class VertexPainterSerializer
    {
    public:
        VertexPainterSerializer();
        explicit VertexPainterSerializer(std::shared_ptr<VertexPainter> to_serialize);

        std::shared_ptr<VertexPainter> get_serialized() const;
        
    private:
        friend class cereal::access;
        template<class Archive>
        void save(Archive& ar, const std::uint32_t) const
            {
                Expects(serialized_);

                std::uint32_t version = 0; // ignored
                
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
            }
    
        template<class Archive>
        void load(Archive& ar, const std::uint32_t)
            {
                std::uint32_t version = 0; // ignored

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
            }

        std::shared_ptr<VertexPainter> serialized_;
    };
}

#endif // VERTEX_PAINTER_SERIALIZER_H
