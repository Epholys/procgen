#ifndef HELPER_COLOR_H
#define HELPER_COLOR_H


#include <SFML/Graphics.hpp>

// TODO add more functions scattered in the code
namespace colors
{
// Returns a Black or White color depending on which as the better constrast
// with 'color'.
sf::Color bw_contrast_color(const sf::Color& color);
} // namespace colors


#endif // HELPER_COLOR_H
