#ifndef POLYMORPHIC_SERIALIZER_H
#define POLYMORPHIC_SERIALIZER_H


#include <memory>
#include <map>
#include <string>
#include <functional>

#include "helper_cereal.hpp"



template<class Parent, class Archive>
class PolymorphicSerializer
{
public:
    static void serialize(std::shared_ptr<Parent> ptr, Archive& ar, const std::uint32_t version)
        {
            std::string type = ptr->get_serialization_type();
            ar(cereal::make_nvp("type", type));
            serialization_fn_.at(type)(ar, version);
        }

    static std::shared_ptr<Parent> deserialize(Archive& ar, const std::uint32_t version)
        {
            std::string type;
            ar(cereal::make_nvp("type", type));
            return deserialization_fn_.at(type)(ar, version);
        }

    using load_fn = std::function<std::shared_ptr<Parent>(Archive, const std::uint32_t)>;
    static std::map<std::string, load_fn> deserialization_fn_;

    using save_fn = std::function<void(Archive, const std::uint32_t)>;
    static std::map<std::string, load_fn> serialization_fn_;
};

template<class Parent, class Archive>
std::map<std::string, typename PolymorphicSerializer<Parent, Archive>::load_fn> PolymorphicSerializer<Parent, Archive>::deserialization_fn_ {};

template<class Parent, class Archive>
std::map<std::string, typename PolymorphicSerializer<Parent, Archive>::save_fn> PolymorphicSerializer<Parent, Archive>::serialization_fn_ {};

#define POLYMORPHIC_SERIALIZER_INIT()                                   \
    virtual std::string get_serialization_type() const                  \
        
#define POLYMORPHIC_SERIALIZER_GET_TYPE(Child)                          \
    virtual std::string get_serialization_type() const                  \
    {                                                                   \
        return #Child;                                                  \
    } 

#define POLYMORPHIC_SERIALIZER_LOAD(Parent, Child)                       \
    template<class Archive>                                             \
    std::shared_ptr<Parent> serializer_load(Archive& ar, const std::uint32_t version) \
    {                                                                   \
        Child* ptr = new Child();                                       \
        ptr->load(ar, version);                                         \
        return std::make_shared<Parent> (ptr);                          \
    }

#define POLYMORPHIC_SERIALIZER(Parent, Child)                           \
    friend class PolymorphicSerializer<Parent, Child>;                  \
    POLYMORPHIC_SERIALIZE_GET_TYPE(Child)                               \
    POLYMORPHIC_SERIALIZE_LOAD(Parent, Child)                           \
    PolymorphicSerializer<Parent, Child>::deserialization_fn[#Child]=serializer_load; \
    PolymorphicSerializer<Parent, Child>::deserialization_fn[#Child]=save;

#endif // POLYMORPHIC_SERIALIZER_H
