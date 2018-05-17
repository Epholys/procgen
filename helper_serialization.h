#ifndef HELPER_SERIALIZATION_H
#define HELPER_SERIALIZATION_H

#include <SFML/Graphics.hpp>
#include "cereal/cereal.hpp"

namespace cereal
{
    template <class Archive, class T,
              traits::EnableIf<traits::is_text_archive<Archive>::value> = traits::sfinae> inline
    void save( Archive & ar, sf::Vector2<T> const& vec )
    {
        ar(cereal::make_nvp("x", vec.x),
           cereal::make_nvp("y", vec.y));
    }

    template <class Archive, class T,
              traits::EnableIf<traits::is_text_archive<Archive>::value> = traits::sfinae> inline
    void save( Archive & ar, sf::Vector2<T>& vec )
    {
        T x, y;
        ar(x, y);
        vec.x = x;
        vec.y = y;
    }
}

#endif // HELPER_SERIALIZATION_H
