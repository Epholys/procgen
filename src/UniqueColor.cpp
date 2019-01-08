#include <iostream>
#include <cmath>
#include <gsl/gsl>
#include "imgui/imgui.h"
#include "UniqueColor.h"

namespace colors
{
    // sf::Color UniqueColor::register_id(int id)
    // {
    //     // Assert precondition : 'id' must be positive and not already registered.
    //     Expects(id >= 0);
    //     auto alread_exists = std::find_if(begin(map_), end(map_),
    //                                       [id](const auto& p){return p.second == id;});
    //     Expects(alread_exists == map_.end());

    //     // Find if there is an available color.
    //     auto empty_slot = std::find_if(begin(map_), end(map_),
    //                                    [](const auto& p){return p.second == -1;});
    //     if (empty_slot != map_.end())
    //     {
    //         // If so, associate 'id' to it.
    //         empty_slot->second = id;
    //         return empty_slot->first;
    //     }
    //     else
    //     {
    //         // Otherwise, generate a new color and create the link.
    //         auto color = new_color();
    //         map_.push_back({color, id});
    //         return color;
    //     }
    // }

    // int UniqueColor::get_id()
    // {
    //     auto empty_slot = std::find_if(begin(map_), end(map_),
    //                                    [](const auto& p)
    //                                    {return std::get<Empty>(p);});
    //     if (empty_slot != end(map_))
    //     {
    //         std::get<Empty>(*empty_slot) = false;
    //         return std::get<Id>(*empty_slot);
    //     }
    //     else
    //     {
    //         map_.push_back({new_color(), current_id_, false});
    //         return current_id_++;
    //     }
    // }
    
    // void UniqueColor::remove_id(int id)
    // {
    //     // Assert precondition: 'id' must have been registered.
    //     auto it = std::find_if(begin(map_), end(map_),
    //                            [id](const auto& p)
    //                            {return std::get<Id>(p) == id &&
    //                                    !std::get<Empty>(p);});
    //     Expects(it != end(map_));

    //     // Mark the color as available.
    //     std::get<Empty>(*it) = true;;
    // }
    sf::Color UniqueColor::get_color(int id)
    {
        Expects(id >= 0 && id <= long(colors_.size()));
        if (id == long(colors_.size()))
        {
            return new_color();
        }
        else
        {
            return colors_.at(id);
        }
    }

    sf::Color UniqueColor::new_color()
    {
        float hue = 0.;

        // For the first pass, subdivide the [0,1] range in 5 values.
        if (pass_ == 0)
        {
            hue = 20.*iter_ / 100.;
            ++iter_;
            if (iter_ == 5)
            {
                ++pass_;
                iter_ = 0;
            }
        }
        // For the other pass, 'hue' is the middle value between two
        // subdivisions of the previous pass.
        else
        {
            hue = (20./std::pow(2, pass_)) * (2*iter_+1) / 100.;
            ++iter_;
            if (iter_ == 5 * std::pow(2, pass_-1))
            {
                ++pass_;
                iter_ = 0;
            }
        }

        float r, g, b;
        ImGui::ColorConvertHSVtoRGB(hue, .8, .8, r, g, b);
        sf::Color new_color (r*255, g*255, b*255);
        colors_.push_back(new_color);
        return  new_color;
    }
}
