#include "PolymorphicSerializer.h"

template<class Parent, class Archive>
typename PolymorphicSerializer<Parent, Archive>::map_save_fn PolymorphicSerializer<Parent, Archive>::serialization_fn_ = {};

template<class Parent, class Archive>
typename PolymorphicSerializer<Parent, Archive>::map_load_fn PolymorphicSerializer<Parent, Archive>::deserialization_fn_ = {};

