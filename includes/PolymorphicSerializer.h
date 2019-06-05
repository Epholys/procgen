#ifndef POLYMORPHIC_SERIALIZER_H
#define POLYMORPHIC_SERIALIZER_H


#include <memory>
#include <map>
#include <string>
#include <functional>

#include "helper_cereal.hpp"
#include "cereal/archives/json.hpp"



template<class Parent>
class PolymorphicSerializer
{
public:
    static void serialize(std::shared_ptr<Parent> ptr, cereal::JSONOutputArchive& ar, const std::uint32_t version)
        {
            ptr->serializer_save(ar, version);
        }
    
    static std::shared_ptr<Parent> deserialize(cereal::JSONInputArchive& ar, const std::uint32_t version)
        {
            std::string type;
            ar(cereal::make_nvp("type", type));
            return deserialization_fn_.at(type)(ar, version);
        }
    using load_fn = std::function<std::shared_ptr<Parent>(cereal::JSONInputArchive, const std::uint32_t)>;
    using map_load_fn = std::map<std::string, load_fn>;
    static void add_deserialization_fn(const std::string& type, load_fn fn)
        {
            deserialization_fn_[type] = fn;
        }
private:
    static inline map_load_fn deserialization_fn_ {};
};

#define POLYMORPHIC_SERIALIZER_INIT(Parent)                          \
    virtual void serializer_save(cereal::JSONOutputArchive& ar,      \
                                 const std::uint32_t version) const; \
    

#define POLYMORPHIC_SERIALIZER_SAVE(Child)                              \
    virtual void serializer_save(cereal::JSONOutputArchive& ar,          \
                                 const std::uint32_t version) const override \
    {                                                                   \
        ar("type", #Child);                                             \
        save(ar, version);                                              \
    }

#define POLYMORPHIC_SERIALIZER_LOAD(Parent, Child)                      \
    static inline std::shared_ptr<Parent> serializer_load(cereal::JSONInputArchive& ar, \
                                                          const std::uint32_t version) \
    {                                                                   \
        Child* ptr = new Child();                                       \
        ptr->load(ar, version);                                         \
        return std::make_shared<Parent> (ptr);                          \
    }

#define POLYMORPHIC_SERIALIZER(Parent, Child)                           \
    friend class PolymorphicSerializer<Parent>;                         \
    POLYMORPHIC_SERIALIZER_SAVE(Child)                                  \
    POLYMORPHIC_SERIALIZER_LOAD(Parent, Child)                          \
    PolymorphicSerializer<Parent>::add_deserialization_fn(#Child, serializer_load); \

#endif // POLYMORPHIC_SERIALIZER_H
