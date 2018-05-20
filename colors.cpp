#include <iostream>
#include <cmath>
#include <gsl/gsl>
#include "imgui/imgui.h"
#include "colors.h"

namespace colors
{
    sf::Color UniqueColor::register_id(int id)
    {
        auto alread_exists = std::find_if(begin(map_), end(map_),
                                          [id](const auto& p){return p.second == id;});
        Expects(alread_exists == map_.end());

        auto empty_slot = std::find_if(begin(map_), end(map_),
                                       [](const auto& p){return p.second == -1;});
        if (empty_slot != map_.end())
        {
            empty_slot->second = id;
            return color_pool_.at(empty_slot->first);
        }
        else
        {
            auto color = new_color();
            map_[color_pool_.size()] = id;
            color_pool_.push_back(color);
            return color;
        }
    }
    
    void UniqueColor::remove_id(int id)
    {
        auto it = std::find_if(begin(map_), end(map_),
                               [id](const auto& p){return p.second == id;});
        Expects(it != end(map_));

        it->second = -1;        
    }
    sf::Color UniqueColor::get_color(int id)
    {
        auto it = std::find_if(begin(map_), end(map_),
                               [id](const auto& p){return p.second == id;});
        Expects(it != end(map_));

        unsigned index = it->first;
        Expects(index < color_pool_.size());
        return color_pool_.at(index);
    }

    sf::Color UniqueColor::new_color()
    {
        float hue = 0.;

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

        std::cout << hue << std::endl;
        
        float r, g, b;
        ImGui::ColorConvertHSVtoRGB(hue, .8, .8, r, g, b);
        return sf::Color(r*255, g*255, b*255);
    }
}
