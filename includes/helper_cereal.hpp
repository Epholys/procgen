#ifndef HELPER_CEREAL_HPP
#define HELPER_CEREAL_HPP


#include <SFML/Graphics.hpp>
#include "cereal/cereal.hpp"

namespace cereal
{
    template <class Archive, class N,
              traits::EnableIf<traits::is_text_archive<Archive>::value> = traits::sfinae> inline
    void serialize(Archive& ar, sf::Vector2<N>& vec)
    {
        ar(cereal::make_nvp("x", vec.x),
           cereal::make_nvp("y", vec.y));
    }
}





#endif // HELPER_CEREAL_HPP
